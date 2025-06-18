#ifndef TTS_GD_SPEECH_SYNTHESIZER_NODE_H
#define TTS_GD_SPEECH_SYNTHESIZER_NODE_H

#include "lexicon.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include "phoneme.h"

namespace godot {

class SpeechSynthesizerNode : public Node {
	GDCLASS(SpeechSynthesizerNode, Node);

	private:
		Ref<Lexicon> lexicon;


	protected:
		static void _bind_methods();
		
	public:
		SpeechSynthesizerNode();
		~SpeechSynthesizerNode();

		String text_to_phoneme_string(const String &text) const;
		TypedArray<Phoneme> text_to_phoneme_array(const String &text) const;

		Ref<Lexicon> get_lexicon() const;
		void set_lexicon(const Ref<Lexicon> &lexicon);

};
}

#endif // TTS_GD_SPEECH_SYNTHESIZER_NODE_H
