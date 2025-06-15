
from dataclasses import dataclass
from typing import Literal
import wordfreq


Stress = Literal[0, 1, 2]

@dataclass
class DictEntry:
	word: str
	phonemes: list[str]
	stress_pattern: list[Stress]
	frequency: float


class DataSet:
	def __init__(self, file_path: str):
		self.file_path = file_path
		self.data: dict[str, DictEntry] = {}
		self.ordered_words: list[str] = []
		self.load_cmu_dict(self.file_path)

	def create_dict_entry(self,line: str) -> DictEntry:
		parts = line.strip().split()
		word = parts[0].strip()
		pre_phonemes = parts[1:]
		phonemes = []
		stress_pattern = []
		current_stress = 0
		for phoneme in pre_phonemes:
			if phoneme[-1].isdigit():
				current_stress = int(phoneme[-1])
				phoneme = phoneme[:-1]
			phonemes.append(phoneme.strip())
			stress_pattern.append(current_stress)
		return DictEntry(
			word=word, 
			phonemes=phonemes, 
			stress_pattern=stress_pattern,
			frequency=wordfreq.word_frequency(word, 'en')
		)

	def load_cmu_dict(self,path: str):
		self.data = {}
		with open(path, 'r', encoding='utf-8') as f:
			for line in f:
				if line.startswith(';;;'):
					continue  # Skip comments
				if ")" in line:
					continue # Skip alternative pronunciations
				if "#" in line:
					line = line.split("#")[0].strip()  # Remove 
				if not line.strip():
					continue  # Skip empty lines
				entry = self.create_dict_entry(line)
				self.data[entry.word] = entry
		self.ordered_words = sorted(self.data.keys(), key=lambda w: self.data[w].frequency, reverse=True)