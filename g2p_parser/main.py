
import random
from src.dataset import DataSet
from src.ruleset import RuleSet




def main():

	cmu_dict_path = "g2p_parser/data/cmudict_full.dict"


	dataset = DataSet(cmu_dict_path)
	ruleset = RuleSet()

	SAMPLE_SIZE = len(dataset.ordered_words) // 4

	total_words: int = 0
	max_possible_phoneme_mismatch: int = 0
	correct_words: int = 0
	mismatched_phonemes: int = 0

	error_words: list[str] = []
	error_patterns = {}

	top_half_words = dataset.ordered_words[:len(dataset.ordered_words)]


	## Pick a few random words from the dataset
	for _ in range(SAMPLE_SIZE):
		word = random.choice(top_half_words)
		entry = dataset.data[word]
		phonemes = ruleset.apply_rules(word)



		if phonemes == entry.phonemes:
			correct_words += 1
		else:
			error_words.append((word, entry.phonemes, phonemes))


		max_possible_phoneme_mismatch += max(len(entry.phonemes), len(phonemes))

		phoneme_errors = []
		for j in range(max(len(entry.phonemes), len(phonemes))):
			if j >= len(entry.phonemes) or j >= len(phonemes):
				mismatched_phonemes += 1
				phoneme_errors.append((j, "missing" if j >= len(entry.phonemes) else "extra"))
			elif entry.phonemes[j] != phonemes[j]:
				mismatched_phonemes += 1
				phoneme_errors.append((j, f"expected {entry.phonemes[j]}, got {phonemes[j]}"))

				if j < len(word):
					start = max(0, j-1)
					end = min(len(word), j+1)
					context = word[start:end]
					if context not in error_patterns:
						error_patterns[context] = {"count": 0, "examples": []}
					error_patterns[context]["count"] += 1
					if len(error_patterns[context]["examples"]) < 5:
						error_patterns[context]["examples"].append((word, entry.phonemes, phonemes))

		total_words += 1
		
	accuracy = correct_words / total_words * 100
	phoneme_mistmatch = mismatched_phonemes / max_possible_phoneme_mismatch * 100 if max_possible_phoneme_mismatch > 0 else 0
	
	
	
	
    # Print error analysis
	print("\nMost Common Error Patterns:")
	for context, data in sorted(error_patterns.items(), key=lambda x: x[1]["count"], reverse=True)[:10]:
		print(f"  '{context}' - {data['count']} errors")
		for word in data["examples"][:3]:
			print(f"    Example: '{word[0]}'")
			print(f"	  Expected: {' '.join(word[1])}")
			print(f"	  Got:      {' '.join(word[2])}")
			
	print("\nHardest Words:")
	# Sort error words by number of phoneme errors
	worst_words = sorted(error_words, 
						key=lambda x: sum(1 for i in range(min(len(x[1]), len(x[2]))) if x[1][i] != x[2][i]), 
						reverse=True)[:5]
	for word, expected, actual in worst_words:
		print(f"  '{word}':")
		print(f"    Expected: {' '.join(expected)}")
		print(f"    Got:      {' '.join(actual)}")

	print(f"Word Accuracy: {accuracy:.2f}% ({correct_words}/{total_words})")
	print(f"Phoneme Mismatch: {phoneme_mistmatch:.2f}% ({mismatched_phonemes}/{max_possible_phoneme_mismatch})")


if __name__ == "__main__":
	main()