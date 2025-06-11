import torch
from model import G2PModel

model = G2PModel(checkpoint="./g2p_parser/best_checkpoint.pt")
model.eval()

print("Model loaded. Trying typing a sentence! Type (Q) to quit.")

while True:
	text = input("> ")
	if text.lower() == "(q)":
		break

	phonemes = model(text)

	print(f"Phonemes: {phonemes}")