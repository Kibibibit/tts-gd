extends Control

@onready
var player: AudioStreamPlayer = $AudioStreamPlayer
@onready
var button: Button = $Button

var speech_synth: SpeechSynth
var generator: AudioStreamGenerator
var playback: AudioStreamGeneratorPlayback



func _ready():
	generator = player.stream as AudioStreamGenerator
	speech_synth = SpeechSynth.new()
	speech_synth.set_voiced_shape(0.1)
	speech_synth.set_sample_rate(player.stream.mix_rate)
	speech_synth.set_fundamental_frequency(120.0)
	button.pressed.connect(_play)


func _play():
	player.play()
	playback = player.get_stream_playback()

	var phoneme_names: Array = ["b", "a","m"]

	var phonemes: Array[Phoneme] = []
	for phoneme_name in phoneme_names:
		phonemes.append(Phonemes.get_phoneme(phoneme_name))

	
	speech_synth.set_phonemes_to_process(phonemes)


func _process(_delta: float) -> void:
	if (playback):

		var frames := playback.get_frames_available()

		if frames > 0:
			for i in range(frames):
				var source: float
				
				if speech_synth.is_finished():
					source = 0.0
				else:
					source = speech_synth.get_next_sample()
					
				var frame := Vector2.ONE*source;
				playback.push_frame(frame)

		
		

	
