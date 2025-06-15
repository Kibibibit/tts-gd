from src.rule import Rule, RuleOuput

# Common, basic rules to get started with.
STARTER_RULES = [
	"_[a]_=>AE",
	"_[b]_=>B IY",
	"_[c]_=>S IY",
	"_[d]_=>D IY",
	"_[e]_=>IY",
	"_[f]_=>EH F",
	"_[g]_=>JH IY",
	"_[h]_=>EY CH",
	"_[i]_=>AY",
	"_[j]_=>JH EY",
	"_[k]_=>K EY",
	"_[l]_=>EH L",
	"_[m]_=>EH M",
	"_[n]_=>EH N",
	"_[o]_=>OW",
	"_[p]_=>P IY",
	"_[q]_=>K Y UW",
	"_[r]_=>AA R",
	"_[s]_=>EH S",
	"_[t]_=>T IY",
	"_[u]_=>Y UW",
	"_[v]_=>V IY",
	"_[w]_=>D AH B AH L Y UW",
	"_[x]_=>EH K S",
	"_[y]_=>W AY",
	"_[z]_=>Z IY",
	"[a]=>AH",
	"[b]=>B",
	"[c]=>K",
	"[d]=>D",
	"[e]=>EH",
	"[f]=>F",
	"[g]=>G",
	"[h]=>HH",
	"[i]=>IH",
	"[j]=>JH",
	"[k]=>K",
	"[l]=>L",
	"[m]=>M",
	"[n]=>N",
	"[o]=>OW",
	"[p]=>P",
	"[q]=>K",
	"[r]=>R",
	"[s]=>S",
	"[t]=>T",
	"[u]=>AH",
	"[v]=>V",
	"[w]=>W",
	"[x]=>K S",
	"[y]=>Y",
	"[z]=>Z",
	"_[ch]r=>K",
	"C[s]_=>Z",
	"K[e]_=>",
	"[tion]=>SH AH N",
	"[sion]=>ZH AH N",
	"C[a]K=>AE",
	"[c]e=>S",
	"[c]i=>S",
	"[c]y=>S",
	"[uni]=>Y UW N IY",
	"[unim]=>AH N IH M",
	"[unin]=>AH N IH N",
	"[ma]=> M AA",
	"[er]=>ER",
	"[ch]=>CH",
	"[sh]=>SH",
	"[th]=>TH",
	"[oy]=>OY",
	"[ow]=>OW",
	"[own]=>OW N",
	"i[ng]=>NG",
	"[wh]=>W",
	"[qu]=>K W",
	"[ph]=>F",
	"[zh]=>ZH",
	"[kn]=>N",
	"[gh]=>F",
	"[ck]=>K",
	"[sc]=>S",
	"[sch]=>SH",
	"[sch]V=>S K",
	"[re]_=>R IH",
	"[bb]=>B",
	"[dd]=>D",
	"[ff]=>F",
	"[gg]=>G",
	"[ll]=>L",
	"[mm]=>M",
	"[ght]=>T",
	"[nn]=>N",
	"[pp]=>P",
	"[rr]=>R",
	"[tt]=>T",
	"[rey]_=> R IY",
	"[gy]=>JH IY",
	"[tted]=>T IH D",
	"[ss]=>S",
	"C[a]ke=>EY",
	"[ur]=>ER",
	"[zz]=>Z",
	"[cc]=>K",
	"[aa]=>AA",
	"m[pu]t=>P Y UW",
	"[ee]=>IY",
	"[ii]=>AY",
	"[oo]=>UW",
	"[hy]=>HH AY",
	"[hyd]=>HH AY D",
	"y[ch]o=>K",
	"[psy]=>S AY",
	"[uu]=>Y UW",
	"[ai]=>AY",
	"[au]=>AW",
	"[os]=> AO Z",
	"[ve]=>V",
	"[me]_=>M",
	"[sian]=>ZH AH N",
	"[ea]=>IY",
	"[ie]=>AY",
	"*[gen]=>JH EH N",
	"_[gen]=>G EH N",
	"[ou]=>OW",
	"[neuro]=>N UH R OW",
	"[ism]=>IH Z AH M",
	"[nese]=>N IY Z",
	"[ies]=>IY Z",
	"[krew]=>K R UW",
	"[dy]_=>D IY",
	"_[dy]s=>D IY",
	"_[dy]n=>D IH",
	"[ch]l=>K",
	"[ae]=>AH",
	"[crew]=>K R UW",
	"[oo]=>UW",
	"[czech]=>CH EH K",
	"[rie]*=>R IY",
	"[emu]=>EH M Y UW",
	'[cia]=>SH AH',
	"[special]=>S P EH SH AH L",
	"[exh]=>IH G Z",
	"[af] => AE F",
	"[ere]=> EH R",
	"[gist]_=>JH IH S T",
	"[a]tion=>EY",
	"[sever]*=>S EH V R",
	"[eye]=>AY",
	"c[ally]=>AE L IY",
	"*[les]_=>L Z",
	"[age]=>EY JH",
	"*[age]=> IH JH",
	"[cu]=>K Y UW",
	"K[y]_=>IY",
	"[ay]=>AY",
	"[psu]=>S UW",
	"_[re]*=>R IY",
	"[mmu]=>M Y UW",
	"[main]=>M EY N",
	"[our]n=>ER",
	"[our]m=>ER",
	"[ry]=>ER IY",
	"o[ph]th=>TH",
	"_[mc]=>M AH K",
]

class RuleSet:

	def __init__(self):
		## Map rules by the first letter of the grapheme
		self.rules: dict[str, list[Rule]] = {}

		## Add starter rules
		for rule_str in STARTER_RULES:
			rule = Rule.from_rule_string(rule_str)
			self.add_rule(rule)

	def add_rule(self, rule: Rule):
		first_letter = rule.grapheme[0]
		if first_letter not in self.rules:
			self.rules[first_letter] = []
		self.rules[first_letter].append(rule)

	def get_rules(self, first_letter: str) -> list[Rule]:
		"""Get rules that start with the given first letter."""
		return self.rules.get(first_letter, [])

	def apply_rules(self, text: str) -> list[str]:

		consumed_text = ""
		unconsumed_text = text
		output_phonemes = []

		while unconsumed_text:
			first_letter = unconsumed_text[0].lower()
			rules = self.get_rules(first_letter)
			if unconsumed_text.startswith(" ") or not rules:
				# If the first character is a space, just consume it
				consumed_text += unconsumed_text[0]
				unconsumed_text = unconsumed_text[1:]
				continue
			
			# Apply rules, keeping the result of the highest scoring rule
			best_result: RuleOuput = None
			for rule in rules:
				result = rule.apply(consumed_text, unconsumed_text)
				if result.score > -1 and (best_result is None or result.score > best_result.score):
					best_result = result
			if best_result is None:
				raise ValueError(f"No applicable rules found for text: {unconsumed_text}")
			
			consumed_text = best_result.new_consumed_text
			unconsumed_text = best_result.new_text
			output_phonemes.extend(best_result.phonemes)

		return output_phonemes
