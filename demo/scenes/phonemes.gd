extends Node

var phonemes: Dictionary[String, Phoneme] = {}

# F = frequency, b = bandwidth, q = quality
const aa_formants_data: Array[Dictionary] = [
	{'f': 730.0, 'b': 50.0, 'g': 0.0},
	{'f': 1090.0, 'b': 50.0, 'g': -3.0},
	{'f': 2440.0, 'b': 50.0, 'g': -6.0},
	{'f': 3200.0, 'b': 50.0, 'g': -9.0}
]

# ɐː as in cart or far
const a_formants_data: Array[Dictionary] = [
	{'f': 700.0, 'b': 50.0, 'g': 0.5},
	{'f': 1100.0, 'b': 50.0, 'g': 0.5},
	{'f': 2400.0, 'b': 50.0, 'g': 0.5},
	{'f': 3200.0, 'b': 50.0, 'g': 0.5}
]

# i: as in "fleece" or "see"
const iy_formants_data: Array[Dictionary] = [
	{'f': 280.0, 'b': 50.0, 'g': 0.5},
	{'f': 2250.0, 'b': 50.0, 'g': 0.5},
	{'f': 2890.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# ɪ as in "kit" or "sit"
const ih_formants_data: Array[Dictionary] = [
	{'f': 400.0, 'b': 50.0, 'g': 0.5},
	{'f': 1920.0, 'b': 50.0, 'g': 0.5},
	{'f': 2560.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# e as in "dress" or "head"
const eh_formants_data: Array[Dictionary] = [
	{'f': 550.0, 'b': 50.0, 'g': 0.5},
	{'f': 1770.0, 'b': 50.0, 'g': 0.5},
	{'f': 2490.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# æ as in "trap" or "bad"
const ae_formants_data: Array[Dictionary] = [
	{'f': 660.0, 'b': 50.0, 'g': 0.5},
	{'f': 1720.0, 'b': 50.0, 'g': 0.5},
	{'f': 2410.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# ɒ as in "lot" or "hot"
const aw_formants_data: Array[Dictionary] = [
	{'f': 570.0, 'b': 50.0, 'g': 0.5},
	{'f': 900.0, 'b': 50.0, 'g': 0.5},
	{'f': 2300.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# ʊ as in "foot" or "put"
const uh_formants_data: Array[Dictionary] = [
	{'f': 400.0, 'b': 50.0, 'g': 0.5},
	{'f': 1020.0, 'b': 50.0, 'g': 0.5},
	{'f': 2240.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# u: as in "goose" or "soon"
const uw_formants_data: Array[Dictionary] = [
	{'f': 320.0, 'b': 50.0, 'g': 0.5},
	{'f': 870.0, 'b': 50.0, 'g': 0.5},
	{'f': 2240.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# ə as in "about" (schwa)
const ax_formants_data: Array[Dictionary] = [
	{'f': 500.0, 'b': 50.0, 'g': 0.5},
	{'f': 1400.0, 'b': 50.0, 'g': 0.5},
	{'f': 2300.0, 'b': 50.0, 'g': 0.5},
	{'f': 3500.0, 'b': 50.0, 'g': 0.5}
]

# ɜː as in "nurse" or "bird" 
const er_formants_data: Array[Dictionary] = [
	{'f': 480.0, 'b': 80.0, 'g': 0.4},
	{'f': 1420.0, 'b': 90.0, 'g': 0.4},
	{'f': 2300.0, 'b': 120.0, 'g': 0.3},
	{'f': 3300.0, 'b': 200.0, 'g': 0.2}
]

# Nasal consonants - characterized by nasal formants and antiformants
const m_formants_data: Array[Dictionary] = [
	{'f': 280.0, 'b': 0.1, 'g': -2.0},  # Reduced gain
	{'f': 1100.0, 'b': 0.1, 'g': -3.0}, 
	{'f': 2200.0, 'b': 0.1, 'g': -4.0}, 
	{'f': 3300.0, 'b': 0.1, 'g': -6.0}
]
const m_antiformants_data: Array[Dictionary] = [
	{'f': 800.0, 'b': 0.1, 'g': 0.5},   # Characteristic bilabial antiformant
	{'f': 2500.0, 'b': 0.1, 'g': 0.4}
]

const n_formants_data: Array[Dictionary] = [
	{'f': 280.0, 'b': 100.0, 'g': 0.4},  # Nasal formant
	{'f': 1700.0, 'b': 100.0, 'g': 0.3}, # Higher F2 than 'm'
	{'f': 2300.0, 'b': 130.0, 'g': 0.3},
	{'f': 3300.0, 'b': 150.0, 'g': 0.2}
]
const n_antiformants_data: Array[Dictionary] = [
	{'f': 1000.0, 'b': 80.0, 'g': 0.5},  # Characteristic alveolar antiformant
	{'f': 2800.0, 'b': 100.0, 'g': 0.4}
]

const ng_formants_data: Array[Dictionary] = [
	{'f': 280.0, 'b': 100.0, 'g': 0},  # Nasal formant
	{'f': 2300.0, 'b': 150.0, 'g': -3.0}, # Much higher F2 (velar)
	{'f': 3000.0, 'b': 200.0, 'g': -6.0},
	{'f': 3800.0, 'b': 250.0, 'g': -9.0}
]
const ng_antiformants_data: Array[Dictionary] = [
	{'f': 1400.0, 'b': 90.0, 'g': 0.4},  # Characteristic velar antiformant
	{'f': 3100.0, 'b': 120.0, 'g': 0.3}
]

# Stop consonants
const t_formants_data: Array[Dictionary] = [
	{'f': 500.0, 'b': 200.0, 'g': 0.2},  # Broad, weak formant (release burst)
	{'f': 2000.0, 'b': 250.0, 'g': 0.2}, # Fricative noise components
	{'f': 3500.0, 'b': 300.0, 'g': 0.2},
	{'f': 5000.0, 'b': 350.0, 'g': 0.1}
]

const b_formants_data: Array[Dictionary] = [
	{'f': 200.0, 'b': 100.0, 'g': 1.0},  # Voice bar during closure
	{'f': 1000.0, 'b': 150.0, 'g': 0.8}, # Burst components
	{'f': 2200.0, 'b': 200.0, 'g': 0.7},
	{'f': 3600.0, 'b': 250.0, 'g': 0.2}
]
var m_phoneme: Phoneme = Phoneme.new()
var n_phoneme: Phoneme = Phoneme.new()
var ng_phoneme: Phoneme = Phoneme.new() 
var t_phoneme: Phoneme = Phoneme.new()
var b_phoneme: Phoneme = Phoneme.new()

var aa_phoneme: Phoneme = Phoneme.new()
var a_phoneme: Phoneme = Phoneme.new()
var iy_phoneme: Phoneme = Phoneme.new()
var ih_phoneme: Phoneme = Phoneme.new()
var eh_phoneme: Phoneme = Phoneme.new()
var ae_phoneme: Phoneme = Phoneme.new()
var aw_phoneme: Phoneme = Phoneme.new()
var uh_phoneme: Phoneme = Phoneme.new()
var uw_phoneme: Phoneme = Phoneme.new()
var ax_phoneme: Phoneme = Phoneme.new()
var er_phoneme: Phoneme = Phoneme.new()
var ey_phoneme: Phoneme = Phoneme.new()
var ay_phoneme: Phoneme = Phoneme.new()
var oy_phoneme: Phoneme = Phoneme.new()
var ow_phoneme: Phoneme = Phoneme.new()

# Diphthongs - built from the formant data of the two vowels that make them up
var eeyy_phoneme: Phoneme = Phoneme.new()

func formant_set_from_data(data: Array[Dictionary]) -> FormantSet:
	var formants: FormantSet = FormantSet.new()
	var frequencies: PackedFloat32Array = PackedFloat32Array()
	var bandwidths: PackedFloat32Array = PackedFloat32Array()
	var gains: PackedFloat32Array = PackedFloat32Array()
	for formant_data in data:
		frequencies.append(formant_data['f'])
		bandwidths.append(formant_data['b'])
		gains.append(formant_data['g'])
	formants.set_formant_data(frequencies, bandwidths, gains)
	return formants

func formant_and_antiformant_set_from_data(f: Array[Dictionary], a: Array[Dictionary]) -> FormantSet:
	var formants: FormantSet = FormantSet.new()
	var frequencies: PackedFloat32Array = PackedFloat32Array()
	var bandwidths: PackedFloat32Array = PackedFloat32Array()
	var gains: PackedFloat32Array = PackedFloat32Array()
	var afrequencies: PackedFloat32Array = PackedFloat32Array()
	var abandwidths: PackedFloat32Array = PackedFloat32Array()
	for formant_data in f:
		frequencies.append(formant_data['f'])
		bandwidths.append(formant_data['b'])
		gains.append(formant_data['g'])
	for antiformant_data in a:
		afrequencies.append(antiformant_data['f'])
		abandwidths.append(antiformant_data['b'])
	formants.set_formant_data(frequencies, bandwidths, gains)
	formants.set_antiformant_data(afrequencies, abandwidths)
	return formants

func get_phoneme(symbol: String) -> Phoneme:
	if phonemes.has(symbol):
		return phonemes[symbol]
	else:
		push_error("Phoneme with symbol '%s' not found." % symbol)
		return null

func _ready() -> void:
	# The aa vowel or ɐ, as in cut
	aa_phoneme.set_formant_set(formant_set_from_data(aa_formants_data))
	aa_phoneme.description = "aa or ɐ, as in cut"
	aa_phoneme.symbol = "aa"
	aa_phoneme.ipa_symbol = "ɐ"
	aa_phoneme.voice_mix_ratio = 0.9
	var aa_envelope: Envelope = Envelope.new()
	aa_envelope.attack_time = 0.01
	aa_envelope.decay_time = 0.1
	aa_envelope.sustain_time = 0.2
	aa_envelope.sustain_level = 0.8
	aa_envelope.release_time = 0.1
	aa_phoneme.set_envelope(aa_envelope)
	phonemes[aa_phoneme.symbol] = aa_phoneme

	# The a vowel or ɑ, as in cart or far
	a_phoneme.set_formant_set(formant_set_from_data(a_formants_data))
	a_phoneme.description = "a or ɑ, as in cart or far"
	a_phoneme.symbol = "a"
	a_phoneme.ipa_symbol = "ɑ"
	a_phoneme.voice_mix_ratio = 0.9
	var a_envelope: Envelope = Envelope.new()
	a_envelope.attack_time = 0.01
	a_envelope.decay_time = 0.1
	a_envelope.sustain_time = 0.2
	a_envelope.sustain_level = 0.8
	a_envelope.release_time = 0.1
	a_phoneme.set_envelope(a_envelope)
	phonemes[a_phoneme.symbol] = a_phoneme
	
	# The iy vowel or i:, as in "fleece" or "see"
	iy_phoneme.set_formant_set(formant_set_from_data(iy_formants_data))
	iy_phoneme.description = "iy or i:, as in fleece or see"
	iy_phoneme.symbol = "iy"
	iy_phoneme.ipa_symbol = "i:"
	iy_phoneme.voice_mix_ratio = 0.9
	var iy_envelope: Envelope = Envelope.new()
	iy_envelope.attack_time = 0.01
	iy_envelope.decay_time = 0.1
	iy_envelope.sustain_time = 0.2
	iy_envelope.sustain_level = 0.8
	iy_envelope.release_time = 0.1
	iy_phoneme.set_envelope(iy_envelope)
	phonemes[iy_phoneme.symbol] = iy_phoneme

	# The ih vowel or ɪ, as in "kit" or "sit"
	ih_phoneme.set_formant_set(formant_set_from_data(ih_formants_data))
	ih_phoneme.description = "ih or ɪ, as in kit or sit"
	ih_phoneme.symbol = "ih"
	ih_phoneme.ipa_symbol = "ɪ"
	ih_phoneme.voice_mix_ratio = 0.9
	var ih_envelope: Envelope = Envelope.new()
	ih_envelope.attack_time = 0.01
	ih_envelope.decay_time = 0.1
	ih_envelope.sustain_time = 0.15
	ih_envelope.sustain_level = 0.8
	ih_envelope.release_time = 0.1
	ih_phoneme.set_envelope(ih_envelope)
	phonemes[ih_phoneme.symbol] = ih_phoneme

	# The eh vowel or e, as in "dress" or "head"
	eh_phoneme.set_formant_set(formant_set_from_data(eh_formants_data))
	eh_phoneme.description = "eh or e, as in dress or head"
	eh_phoneme.symbol = "eh"
	eh_phoneme.ipa_symbol = "e"
	eh_phoneme.voice_mix_ratio = 0.9
	var eh_envelope: Envelope = Envelope.new()
	eh_envelope.attack_time = 0.01
	eh_envelope.decay_time = 0.1
	eh_envelope.sustain_time = 0.15
	eh_envelope.sustain_level = 0.8
	eh_envelope.release_time = 0.1
	eh_phoneme.set_envelope(eh_envelope)
	phonemes[eh_phoneme.symbol] = eh_phoneme

	# The ae vowel or æ, as in "trap" or "bad"
	ae_phoneme.set_formant_set(formant_set_from_data(ae_formants_data))
	ae_phoneme.description = "ae or æ, as in trap or bad"
	ae_phoneme.symbol = "ae"
	ae_phoneme.ipa_symbol = "æ"
	ae_phoneme.voice_mix_ratio = 0.9
	var ae_envelope: Envelope = Envelope.new()
	ae_envelope.attack_time = 0.01
	ae_envelope.decay_time = 0.1
	ae_envelope.sustain_time = 0.15
	ae_envelope.sustain_level = 0.8
	ae_envelope.release_time = 0.1
	ae_phoneme.set_envelope(ae_envelope)
	phonemes[ae_phoneme.symbol] = ae_phoneme

	# The aw vowel or ɒ, as in "lot" or "hot"
	aw_phoneme.set_formant_set(formant_set_from_data(aw_formants_data))
	aw_phoneme.description = "aw or ɒ, as in lot or hot"
	aw_phoneme.symbol = "aw"
	aw_phoneme.ipa_symbol = "ɒ"
	aw_phoneme.voice_mix_ratio = 0.9
	var aw_envelope: Envelope = Envelope.new()
	aw_envelope.attack_time = 0.01
	aw_envelope.decay_time = 0.1
	aw_envelope.sustain_time = 0.15
	aw_envelope.sustain_level = 0.8
	aw_envelope.release_time = 0.1
	aw_phoneme.set_envelope(aw_envelope)
	phonemes[aw_phoneme.symbol] = aw_phoneme

	# The uh vowel or ʊ, as in "foot" or "put"
	uh_phoneme.set_formant_set(formant_set_from_data(uh_formants_data))
	uh_phoneme.description = "uh or ʊ, as in foot or put"
	uh_phoneme.symbol = "uh"
	uh_phoneme.ipa_symbol = "ʊ"
	uh_phoneme.voice_mix_ratio = 0.9
	var uh_envelope: Envelope = Envelope.new()
	uh_envelope.attack_time = 0.01
	uh_envelope.decay_time = 0.1
	uh_envelope.sustain_time = 0.15
	uh_envelope.sustain_level = 0.8
	uh_envelope.release_time = 0.1
	uh_phoneme.set_envelope(uh_envelope)
	phonemes[uh_phoneme.symbol] = uh_phoneme

	# The uw vowel or u:, as in "goose" or "soon"
	uw_phoneme.set_formant_set(formant_set_from_data(uw_formants_data))
	uw_phoneme.description = "uw or u:, as in goose or soon"
	uw_phoneme.symbol = "uw"
	uw_phoneme.ipa_symbol = "u:"
	uw_phoneme.voice_mix_ratio = 0.9
	var uw_envelope: Envelope = Envelope.new()
	uw_envelope.attack_time = 0.01
	uw_envelope.decay_time = 0.1
	uw_envelope.sustain_time = 0.2
	uw_envelope.sustain_level = 0.8
	uw_envelope.release_time = 0.1
	uw_phoneme.set_envelope(uw_envelope)
	phonemes[uw_phoneme.symbol] = uw_phoneme

	# The ax vowel or ə, as in "about" (schwa)
	ax_phoneme.set_formant_set(formant_set_from_data(ax_formants_data))
	ax_phoneme.description = "ax or ə, as in about (schwa)"
	ax_phoneme.symbol = "ax"
	ax_phoneme.ipa_symbol = "ə"
	ax_phoneme.voice_mix_ratio = 0.9
	var ax_envelope: Envelope = Envelope.new()
	ax_envelope.attack_time = 0.005
	ax_envelope.decay_time = 0.05
	ax_envelope.sustain_time = 0.15
	ax_envelope.sustain_level = 0.7
	ax_envelope.release_time = 0.05
	ax_phoneme.set_envelope(ax_envelope)
	phonemes[ax_phoneme.symbol] = ax_phoneme

	# The er vowel or ɜː, as in "nurse" or "bird"
	er_phoneme.set_formant_set(formant_set_from_data(er_formants_data))
	er_phoneme.description = "er or ɜː, as in nurse or bird"
	er_phoneme.symbol = "er"
	er_phoneme.ipa_symbol = "ɜː"
	er_phoneme.voice_mix_ratio = 0.9
	var er_envelope: Envelope = Envelope.new()
	er_envelope.attack_time = 0.01
	er_envelope.decay_time = 0.1
	er_envelope.sustain_time = 0.2
	er_envelope.sustain_level = 0.8
	er_envelope.release_time = 0.1
	er_phoneme.set_envelope(er_envelope)
	phonemes[er_phoneme.symbol] = er_phoneme

	# The ey vowel or eɪ, as in "face" or "day"
	eeyy_phoneme.set_formant_set(formant_set_from_data(eh_formants_data))
	eeyy_phoneme.set_end_formant_set(formant_set_from_data(uw_formants_data))
	eeyy_phoneme.symbol = "eeyy"
	eeyy_phoneme.ipa_symbol = "eɪ"
	eeyy_phoneme.voice_mix_ratio = 0.9
	eeyy_phoneme.description = "ey or eɪ, as in face or day"
	eeyy_phoneme.set_is_diphthong(true)
	var eeyy_envelope: Envelope = Envelope.new()
	eeyy_envelope.attack_time = 0.01
	eeyy_envelope.decay_time = 0.1
	eeyy_envelope.sustain_time = 0.1
	eeyy_envelope.sustain_level = 0.8
	eeyy_envelope.release_time = 0.1
	eeyy_phoneme.set_envelope(eeyy_envelope)
	phonemes[eeyy_phoneme.symbol] = eeyy_phoneme
	eeyy_phoneme.transition_start = 0.1
	eeyy_phoneme.transition_length = 0.05

	m_phoneme.set_formant_set(formant_and_antiformant_set_from_data(m_formants_data, m_antiformants_data))
	m_phoneme.description = "m, bilabial nasal consonant"
	m_phoneme.symbol = "m"
	m_phoneme.ipa_symbol = "m"
	m_phoneme.voice_mix_ratio = 0.9  # Almost fully voiced
	var m_envelope: Envelope = Envelope.new()
	m_envelope.attack_time = 0.02
	m_envelope.decay_time = 0.03
	m_envelope.sustain_time = 0.08
	m_envelope.sustain_level = 0.7  # Lower amplitude than vowels
	m_envelope.release_time = 0.04
	m_phoneme.set_envelope(m_envelope)
	phonemes[m_phoneme.symbol] = m_phoneme
	
	# 'n' alveolar nasal consonant
	n_phoneme.set_formant_set(formant_and_antiformant_set_from_data(n_formants_data, n_antiformants_data))
	n_phoneme.description = "n, alveolar nasal consonant"
	n_phoneme.symbol = "n"
	n_phoneme.ipa_symbol = "n"
	n_phoneme.voice_mix_ratio = 0.9  # Almost fully voiced
	var n_envelope: Envelope = Envelope.new()
	n_envelope.attack_time = 0.02
	n_envelope.decay_time = 0.03
	n_envelope.sustain_time = 0.08
	n_envelope.sustain_level = 0.7  # Lower amplitude than vowels
	n_envelope.release_time = 0.04
	n_phoneme.set_envelope(n_envelope)
	phonemes[n_phoneme.symbol] = n_phoneme
	
	# 'ng' velar nasal consonant
	ng_phoneme.set_formant_set(formant_and_antiformant_set_from_data(ng_formants_data, ng_antiformants_data))
	ng_phoneme.description = "ng, velar nasal consonant as in 'sing'"
	ng_phoneme.symbol = "ng"
	ng_phoneme.ipa_symbol = "ŋ"
	ng_phoneme.voice_mix_ratio = 0.9  # Almost fully voiced
	var ng_envelope: Envelope = Envelope.new()
	ng_envelope.attack_time = 0.02
	ng_envelope.decay_time = 0.03
	ng_envelope.sustain_time = 0.08
	ng_envelope.sustain_level = 0.7  # Lower amplitude than vowels
	ng_envelope.release_time = 0.05  # Slightly longer release than other nasals
	ng_phoneme.set_envelope(ng_envelope)
	phonemes[ng_phoneme.symbol] = ng_phoneme
	
	# 't' voiceless alveolar stop
	t_phoneme.set_formant_set(formant_set_from_data(t_formants_data))
	t_phoneme.description = "t, voiceless alveolar stop"
	t_phoneme.symbol = "t"
	t_phoneme.ipa_symbol = "t"
	t_phoneme.voice_mix_ratio = 0.1  # Mostly unvoiced (just the burst)
	var t_envelope: Envelope = Envelope.new()
	t_envelope.attack_time = 0.001  # Very fast attack (plosive burst)
	t_envelope.decay_time = 0.02
	t_envelope.sustain_time = 0.01  # Very short sustain
	t_envelope.sustain_level = 0.6
	t_envelope.release_time = 0.03  # Aspirated release
	t_phoneme.set_envelope(t_envelope)
	phonemes[t_phoneme.symbol] = t_phoneme
	
	# 'b' voiced bilabial stop
	b_phoneme.set_formant_set(formant_set_from_data(b_formants_data))
	b_phoneme.description = "b, voiced bilabial stop"
	b_phoneme.symbol = "b"
	b_phoneme.ipa_symbol = "b"
	b_phoneme.voice_mix_ratio = 0.7  # Partially voiced (voice bar + burst)
	var b_envelope: Envelope = Envelope.new()
	b_envelope.attack_time = 0.005  # Fast attack
	b_envelope.decay_time = 0.02
	b_envelope.sustain_time = 0.02  # Short sustain
	b_envelope.sustain_level = 0.7
	b_envelope.release_time = 0.01  # Less aspirated than 't'
	b_phoneme.set_envelope(b_envelope)
	phonemes[b_phoneme.symbol] = b_phoneme
