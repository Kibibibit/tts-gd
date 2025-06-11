## Based heavily on the work of Nicholas Hinds,
## Their repo can be found here:
## https://github.com/nicholashinds/g2p-model


import torch 
import torch.nn as nn
from torch.nn.utils.rnn import pad_sequence
from torch.utils.data import DataLoader, Dataset
from tqdm import tqdm
import random

from model import G2PModel

def load_cmu_data(cmu_file):
	data = []
	with open(cmu_file, "r") as file:
		for line in file:
			if not line.strip() or line.startswith("#"):
				continue
			line = line.split("#")[0].strip()
			if not line:
				continue
			if "(" in line:
				continue
			split_line = line.split(" ")
			word = split_line[0].lower()
			phonemes = split_line[1:]
			data.append({'word': word, 'phonemes': phonemes})
	return data

class CMUDataset(Dataset):
	def __init__(self, data, model):
		self.data = data
		self.model = model

	def __len__(self):
		return len(self.data)
	
	def __getitem__(self, idx):
		sample = self.data[idx]
		word = sample['word']
		phonemes = sample['phonemes']
		graphemes = list(word.lower()) + ["</s>"]
		input_ids = [
			self.model.grapheme_to_i.get(
				char, self.model.grapheme_to_i["<unk>"]
			) for char in graphemes
		]

		labels = [
			self.model.phoneme_to_i["<s>"]] + \
			[self.model.phoneme_to_i.get(p, self.model.phoneme_to_i["<unk>"]) for p in phonemes] + \
			[self.model.phoneme_to_i["</s>"]]
		
		return {'input_ids': input_ids, 'labels': labels}

def collate_batch(batch, model):
	input_ids = [torch.tensor(sample['input_ids']) for sample in batch]
	labels = [torch.tensor(sample['labels']) for sample in batch]
	
	input_ids_padded = pad_sequence(
		input_ids,
		batch_first=True, 
		padding_value=model.grapheme_to_i["<pad>"]
	)
	labels_padded = pad_sequence(
		labels, 
		batch_first=True, 
		padding_value=model.phoneme_to_i["<pad>"]
	)
	
	return {'input_ids': input_ids_padded, 'labels': labels_padded}

def evaluate(model, data_loader, device):

	model.eval()
	total_loss = 0.0

	criterion = nn.CrossEntropyLoss(ignore_index=model.phoneme_to_i["<pad>"])

	with torch.no_grad():
		for batch in data_loader:
			input_ids = batch['input_ids'].to(device)
			labels = batch['labels'].to(device)

			encoder_outputs, encoder_hidden = model.forward(input_ids)

			decoder_input = torch.full(
				(labels.size(0), 1), model.phoneme_to_i["<s>"], dtype=torch.long).to(device)
			
			max_length = labels.size(1)

			all_outputs = []

			decoder_hidden = encoder_hidden

			for t in range(max_length-1):
				output, decoder_hidden = model.decode_step(decoder_input, decoder_hidden, encoder_outputs)
				all_outputs.append(output)

				decoder_input = labels[:, t+1].unsqueeze(1)

			outputs = torch.stack(all_outputs, dim=1)

			loss = criterion(
				outputs.view(-1, outputs.size(-1)),
				labels[:, 1:].reshape(-1)
			)
			total_loss += loss.item()
	avg_loss = total_loss / len(data_loader)
	return avg_loss

if __name__ == "__main__":

	best_checkpoint_file = "./g2p_parser/best_checkpoint.pt"
	last_checkpoint_file = "./g2p_parser/last_checkpoint.pt"

	checkpoint_path = "./g2p_parser/checkpoint.pt"

	model = G2PModel(checkpoint=checkpoint_path)

	device = torch.device('cpu')

	model.to(device)

	cmu_file = "./g2p_parser/cmudict_full.dict"
	data = load_cmu_data(cmu_file)

	random.shuffle(data)

	split_index = int(0.95 * len(data))

	train_data = data[:split_index]
	val_data = data[split_index:]

	train_dataset = CMUDataset(train_data, model)
	val_dataset = CMUDataset(val_data, model)

	train_loader = DataLoader(
		train_dataset, 
		batch_size=64, 
		shuffle=True, 
		collate_fn=lambda x: collate_batch(x, model)
	)
	val_loader = DataLoader(
		val_dataset, 
		batch_size=64, 
		shuffle=False, 
		collate_fn=lambda x: collate_batch(x, model)
	)
	criterion = nn.CrossEntropyLoss(ignore_index=model.phoneme_to_i["<pad>"])

	optimizer = torch.optim.Adam(model.parameters(), lr=0.0005, weight_decay=1e-3)

	scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
		optimizer, 
		mode='min', 
		factor=0.5, 
		patience=2,
	)

	criterion.to(device)

	num_epochs = 75

	best_val_loss = float('inf')

	patience = 15
	epochs_no_improvement = 0

	scaler = None

	for epoch in range(num_epochs):
		model.train()
		total_loss = 0.0

		for i, batch in enumerate(tqdm(train_loader, desc=f"Epoch {epoch+1}/{num_epochs}")):
			input_ids = batch['input_ids'].to(device)
			labels = batch['labels'].to(device)

			optimizer.zero_grad()

			encoder_outputs, encoder_hidden = model.forward(input_ids)

			decoder_input = labels[:, :-1]
			decoder_targets = labels[:, 1:]

			decoder_hidden = encoder_hidden

			outputs = []

			for t in range(decoder_input.size(1)):
				decoder_input_t = decoder_input[:, t].unsqueeze(1)
				output, decoder_hidden = model.decode_step(
					decoder_input_t, decoder_hidden, encoder_outputs)
				outputs.append(output)
			
			outputs = torch.stack(outputs, dim=1)

			loss = criterion(
				outputs.view(-1, outputs.size(-1)),
				decoder_targets.reshape(-1)
			)

			loss.backward()

			torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=1.0)
			optimizer.step()
			total_loss += loss.item()

		avg_train_loss = total_loss / len(train_loader)
		val_loss = evaluate(model, val_loader, device)

		print(f"Epoch {epoch+1}/{num_epochs}, "
			  f"Train Loss: {avg_train_loss:.4f}, "
			  f"Val Loss: {val_loss:.4f}")
		scheduler.step(val_loss)

		if val_loss < best_val_loss:
			best_val_loss = val_loss
			torch.save(model.state_dict(), best_checkpoint_file)
			epochs_no_improvement = 0
			print(f"New best model saved with loss: {best_val_loss:.4f}")
		else:
			epochs_no_improvement += 1
			print(f"No improvement in validation loss for {epochs_no_improvement} epochs.")
		
		if epochs_no_improvement >= patience:
			print("Early stopping triggered.")
			break
	
	torch.save(model.state_dict(), last_checkpoint_file)
	print(f"Training complete. Best model saved to {best_checkpoint_file}.")


