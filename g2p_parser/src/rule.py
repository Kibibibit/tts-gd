

class RuleOuput:
	def __init__(self, new_consumed_text: str, new_text:str, phonemes: list[str], score:int = 0):
		self.new_consumed_text:str = new_consumed_text
		self.new_text:str = new_text
		self.phonemes:str = phonemes
		self.score:int = score

class Rule:
	def __init__(self):
		self.pre_context = ""
		self.post_context = ""
		self.grapheme = ""
		self.score_bonus: int = 0
		self.phonemes: list[str] = []


	@classmethod
	def from_rule_string(cls, rule_string: str) -> "Rule":
		parts = rule_string.split("=>")
		if len(parts) != 2:
			raise ValueError(f"Invalid rule string: {rule_string}")
		
		context_and_grapheme, phonemes = parts[0].strip(), parts[1].strip()
		phonemes = phonemes.split()
	
		rule = cls()
		# Split the context and grapheme
		# The grapheme will be wrapped in []
		if "[" not in context_and_grapheme or "]" not in context_and_grapheme:
			raise ValueError(f"Invalid rule string, missing grapheme: {rule_string}")
		pre_context, grapheme_and_post_context = context_and_grapheme.split("[", 1)
		if "]" not in grapheme_and_post_context:
			raise ValueError(f"Invalid rule string, missing closing bracket: {rule_string}")
		grapheme, post_context = grapheme_and_post_context.split("]", 1)
		rule.pre_context = pre_context.strip()
		rule.grapheme = grapheme.strip()
		rule.post_context = post_context.strip()
		if not rule.grapheme:
			raise ValueError(f"Grapheme cannot be empty in rule string: {rule_string}")
		if not rule.grapheme.isalpha():
			raise ValueError(f"Grapheme must be alphabetic in rule string: {rule_string}")
		if not rule.pre_context.isalpha() and not all(c in "_*CVK" for c in rule.pre_context):
			raise ValueError(f"Pre-context must be alphabetic or contain special characters in rule string: {rule_string}")
		if not rule.post_context.isalpha() and not all(c in "_*CVK" for c in rule.post_context):
			raise ValueError(f"Post-context must be alphabetic or contain special characters in rule string: {rule_string}")
		rule.phonemes = [p.strip() for p in phonemes if p.strip()]
		return rule


	
	def get_score(self) -> int:
		return len(self.grapheme) + self.score_bonus + len(self.pre_context) + len(self.post_context)
	
	def __letter_is_vowel(self, letter: str, allow_y: bool = False) -> bool:
		if letter in "aeiou":
			return True
		if allow_y and letter == "y":
			return True
		return False
	def __letter_is_consonant(self, letter: str, allow_y: bool = False) -> bool:
		if letter in "bcdfghjklmnpqrstvwxyz":
			return True
		if allow_y and letter == "y":
			return True
		return False
	
	def __letter_is_vowely(self, letter: str) -> bool:
		"""Check if the letter is a vowel or 'y'."""
		return self.__letter_is_vowel(letter, allow_y=True)
	def __letter_is_consonanty(self, letter: str) -> bool:
		"""Check if the letter is a consonant or 'y'."""
		return self.__letter_is_consonant(letter, allow_y=True)

	def pre_context_matches(self, consumed_text: str, verbose: bool = False) -> bool:
		"""Check if the pre-context matches the consumed text."""
		if not self.pre_context:
			if verbose:
				print("Pre-context is empty, matches by default")
			return True
		for _i in range(len(self.pre_context)):
			i = len(self.pre_context) - _i - 1
			ci = len(consumed_text) - _i - 1
			if ci < 0 or ci >= len(consumed_text):
				if self.pre_context[i] == "_":
					if verbose:
						print(f"Pre-context matches at position {i} with word boundary")
					return True
				if verbose:
					print(f"Pre-context does not match at position {i}, consumed text too short")
				return False
			if verbose:
				print(f"Matching letter '{self.pre_context[i]}' with consumed text '{consumed_text[ci]}' at position {i}")
			if self.pre_context[i] == "_":
				if consumed_text[ci] != " ":
					if verbose:
						print(f"Pre-context does not match at position {i}, expected word boundary but got '{consumed_text[ci]}'")
					return False
			elif self.pre_context[i] == "*":
				if verbose:
					print(f"Pre-context matches at position {i} with any character")
				continue
			elif self.pre_context[i] == "V":
				if not self.__letter_is_vowel(consumed_text[ci]):
					if verbose:
						print(f"Pre-context does not match at position {i}, expected vowel but got '{consumed_text[ci]}'")
					return False
			elif self.pre_context[i] == "C":
				if not self.__letter_is_consonanty(consumed_text[ci]):
					if verbose:
						print(f"Pre-context does not match at position {i}, expected consonant or 'y' but got '{consumed_text[ci]}'")
					return False
			elif self.pre_context[i] == "K":
				if not self.__letter_is_consonant(consumed_text[ci]) or consumed_text[ci] == "y":
					if verbose:
						print(f"Pre-context does not match at position {i}, expected consonant but got '{consumed_text[ci]}'")
					return False
			elif self.pre_context[i] == "Y":
				if not self.__letter_is_vowely(consumed_text[ci]) and consumed_text[ci] != "y":
					if verbose:
						print(f"Pre-context does not match at position {i}, expected vowel or 'y' but got '{consumed_text[ci]}'")
					return False
			elif self.pre_context[i] != consumed_text[ci]:
				if verbose:
					print(f"Pre-context does not match at position {i}, expected '{self.pre_context[i]}' but got '{consumed_text[ci]}'")
				return False
		if verbose:
			print("Pre-context matches")
		return True
	
	def post_context_matches(self, text: str, verbose: bool = False) -> bool:
		for i in range(len(self.post_context)):
			if i >= len(text):
				if self.post_context[i] == "_":
					if verbose:
						print(f"Post-context matches at position {i} with word boundary")
					return True
				if verbose:
					print(f"Post-context does not match at position {i}, text too short")
				return False
			if verbose:
				print(f"Matching letter '{self.post_context[i]}' with text '{text[i]}' at position {i}")
			if self.post_context[i] == "_":
				if text[i] != " ":
					if verbose:
						print(f"Post-context does not match at position {i}, expected word boundary but got '{text[i]}'")
					return False
			elif self.post_context[i] == "*":
				if verbose:
					print(f"Post-context matches at position {i} with any character")
				continue
			elif self.post_context[i] == "V":
				if not self.__letter_is_vowel(text[i]):
					if verbose:
						print(f"Post-context does not match at position {i}, expected vowel but got '{text[i]}'")
					return False
			elif self.post_context[i] == "C":
				if not self.__letter_is_consonanty(text[i]):
					if verbose:
						print(f"Post-context does not match at position {i}, expected consonant but got '{text[i]}'")
					return False
			elif self.post_context[i] == "K":
				if not self.__letter_is_consonant(text[i]) or text[i] == "y":
					if verbose:
						print(f"Post-context does not match at position {i}, expected consonant but got '{text[i]}'")
					return False
			elif self.post_context[i] == "Y":
				if not self.__letter_is_vowely(text[i]) and text[i] != "y":
					if verbose:
						print(f"Post-context does not match at position {i}, expected vowel or 'y' but got '{text[i]}'")
					return False
			elif self.post_context[i] != text[i]:
				if verbose:
					print(f"Post-context does not match at position {i}, expected '{self.post_context[i]}' but got '{text[i]}'")
				return False
		if verbose:
			print("Post-context matches")
		return True

	
	
	def apply(self, consumed_text: str, text: str, verbose: bool = False) -> RuleOuput:
		if not self.grapheme:
			raise ValueError("Grapheme must be set before applying the rule")
		
		## Check if the pre-context matches,
		## keeping mind _ means a word boundary
		## V means a vowel, C means a consonant or Y, Y means a vowel or y, K means a consonant and not Y
		## And * means any character

		if not text.startswith(self.grapheme):
			if verbose:
				print(f"Rule '{self.grapheme}' does not match at the start of '{text}'")
			return RuleOuput(consumed_text, text, [], -1)

		if not self.pre_context_matches(consumed_text, verbose=verbose):
			if verbose:
				print(f"Pre-context '{self.pre_context}' does not match for '{text}'")
			return RuleOuput(consumed_text, text, [], -1)
		
		
		new_consumed_text = consumed_text + self.grapheme
		new_text = text[len(self.grapheme):]
		
		if not self.post_context_matches(new_text, verbose=verbose):
			if verbose:
				print(f"Post-context '{self.post_context}' does not match for '{new_text}'")
			return RuleOuput(consumed_text, text, [], -1)
		
		return RuleOuput(
			new_consumed_text, 
			new_text, 
			self.phonemes,
			self.get_score()
		)

		


		
