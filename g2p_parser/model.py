## Based heavily on the work of Nicholas Hinds,
## Their repo can be found here:
## https://github.com/nicholashinds/g2p-model

import torch
import torch.nn as nn
import torch.nn.functional as F
import unicodedata
import regex as re

GRAPHEMES = [
	"<pad>", "<unk>", "/s>"
] + list("abcdefghijklmnopqrstuvwxyz")

PHONEMES = [
	"<pad>", "<unk>", "<s>", "</s>"
] + [
	'AA0', 'AA1', 'AA2', 'AE0', 'AE1', 'AE2', 'AH0', 'AH1', 'AH2',
	'AO0', 'AO1', 'AO2', 'AW0', 'AW1', 'AW2', 'AY0', 'AY1', 'AY2', 'B',
	'CH', 'D', 'DH', 'EH0', 'EH1', 'EH2', 'ER0', 'ER1', 'ER2', 'EY0',
	'EY1', 'EY2', 'F', 'G', 'HH', 'IH0', 'IH1', 'IH2', 'IY0', 'IY1',
	'IY2', 'JH', 'K', 'L', 'M', 'N', 'NG', 'OW0', 'OW1', 'OW2',
	'OY0', 'OY1', 'OY2', 'P', 'R', 'S', 'SH', 'T', 'TH', 'UH0',
	'UH1', 'UH2', 'UW0', 'UW1', 'UW2', 'V', 'W', 'Y', 'Z', 'ZH'
]


class G2PModel(nn.Module):
	def __init__(
			self, 
			embedding_dim: int = 128, 
			hidden_size: int = 128, 
			dropout_rate: float = 0.6,
			checkpoint: str = None
		):
		super(G2PModel, self).__init__()


		self.grapheme_to_i = {g: i for i, g in enumerate(GRAPHEMES)}
		self.i_to_grapheme = {i: g for i, g in enumerate(GRAPHEMES)}
		self.phoneme_to_i = {p: i for i, p in enumerate(PHONEMES)}
		self.i_to_phoneme = {i: p for i, p in enumerate(PHONEMES)}
		
		# Coverts characters to vectors for processing
		self.encoder_embedding = nn.Embedding(len(GRAPHEMES), embedding_dim)

		self.decoder_embedding = nn.Embedding(len(PHONEMES), embedding_dim)

		self.encoder_gru = nn.GRU(embedding_dim, hidden_size, batch_first=True)
		self.decoder_gru = nn.GRU(embedding_dim+hidden_size, hidden_size, batch_first=True)

		# self.attn = nn.Linear(hidden_size+hidden_size, hidden_size)
		# self.attn_combine = nn.Linear(hidden_size+embedding_dim, embedding_dim)

		self.fully_connected = nn.Linear(embedding_dim, len(PHONEMES))

		self.dropout = nn.Dropout(dropout_rate)

		self.load_checkpoint(checkpoint)

	def load_checkpoint(self, checkpoint_path):
		if checkpoint_path is not None:
			checkpoint = torch.load(checkpoint_path, map_location='cpu', weights_only=True)
			self.load_state_dict(checkpoint['model_state_dict'])
			print(f"Loaded model from {checkpoint_path}")
		else:
			print("No checkpoint provided, using random initialization.")
			


	def forward(self, x):

		embedded = self.dropout(self.encoder_embedding(x))

		encoder_output, hidden = self.encoder_gru(embedded)

		return encoder_output, hidden
	
	def decode_step(self, decoder_input, decoder_hidden, encoder_outputs):
		embedded = self.dropout(self.decoder_embedding(decoder_input))

		attn_weights = torch.bmm(decoder_hidden.transpose(0, 1), encoder_outputs.transpose(1,2))

		attn_weights = F.softmax(attn_weights, dim=2)

		context = torch.bmm(attn_weights, encoder_outputs)

		rnn_input = torch.cat((embedded, context), dim=2)

		output, decoder_hidden = self.decoder_gru(rnn_input, decoder_hidden)

		output = self.fully_connected(output.squeeze(1))

		return output, decoder_hidden

	def predict(self, word, beam_width=3):
		self.eval()

		chars = list(word.lower())+["</s>"]

		x = [self.grapheme_to_i.get(c, self.grapheme_to_i["<unk>"]) for c in chars]

		x = torch.tensor(x).unsqueeze(0).to(next(self.parameters()).device)

		encoder_outputs, hidden = self.forward(x)

		decoder_hidden = hidden

		decoder_input = torch.tensor(
			[self.phoneme_to_i["<s>"]],
			device=x.device
		)

		max_length = 50

		beams = [(0, decoder_input, decoder_hidden)]
		completed_beams = []

		for _ in range(max_length):
			new_beams = []

			for score, decoder_input_seq, decoder_hidden in beams:
				last_token = decoder_input_seq[:, -1:]

				if last_token.item() == self.phoneme_to_i["</s>"]:
					completed_beams.append((score, decoder_input_seq))
					continue

				output, decoder_hidden_new = self.decode_step(
					last_token, decoder_hidden, encoder_outputs
				)

				log_probs = torch.log_softmax(output, dim=1)

				top_log_probs, top_indices = log_probs.topk(beam_width)

				for i in range(beam_width):
					new_Score = score + top_log_probs[0, i].item()
					new_decoder_input_seq = torch.cat(
						[decoder_input_seq, top_indices[:, i:i+1]],
						dim=1
					)
					new_beams.append((new_Score, new_decoder_input_seq, decoder_hidden_new))
					
			beams = sorted(new_beams, key=lambda x: x[0], reverse=True)[:beam_width]
		
			if all(decoder_input_seq[0, -1].item() == self.phoneme_to_i["</s>"] for _, decoder_input_seq, _ in beams):
				completed_beams.extend(beams)
				break
				
		completed_beams.extend(beams)

		best_beam = max(completed_beams, key=lambda x: x[0])

		output_indices = best_beam[1].squeeze().tolist()[1:]

		if self.phoneme_to_i["</s>"] in output_indices:
			end_index = output_indices.index(self.phoneme_to_i["</s>"])
			output_indices = output_indices[:end_index]
		
		preds = [self.i_to_phoneme[i] for i in output_indices]

		return preds
	
	def normalize_text(self, text):

		text = ''.join(
			char for char in unicodedata.normalize('NDF', text)
			if unicodedata.category(char) != 'Mn'
		).lower()

		text = re.sub(r"[^ a-z']", "", text)

		return text.strip()
	
	def __call__(self, text):
		normalized_text = self.normalize_text(text)
		
		words = normalized_text.split(" ")
		output = ""
		for word in words:
			if word:
				pred = self.predict(word)
				output += " ".join(pred) + " "
		return output.strip()
