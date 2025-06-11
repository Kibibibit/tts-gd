
from wordfreq import word_frequency


DICT_FILE: str = "./g2p_parser/cmudict_full.dict"

def load_cmu_file(file_path) -> tuple[dict, dict, dict]:
	cmu_dict = {}
	lexicon = {}
	single_letter_words = {}
	with open(file_path, "r") as file:

		validLines: int = 0
		parsedLines: int = 0

		for line in file:
			parsedLines += 1
			if not line.strip() or line.startswith("#"):
				continue

			line = line.split("#")[0].strip()

			if not line:
				continue
			


			validLines += 1

			split_line = line.split(" ")

			word = split_line[0].lower()
	
			phonemes = split_line[1:]
			phonemes = " ".join(phonemes).strip()
			cmu_dict[word] = phonemes
			if len(word) == 1:
				single_letter_words[word] = phonemes

			# ignore lines that contain variants eg (2) or (3) when writing to the lexicon
			if "(" in line:
				continue

			lexicon[word] = word_frequency(word, "en")





		print("Read file. {}/{} lines were valid.".format(validLines, parsedLines))
		return cmu_dict, lexicon, single_letter_words
		

