# Unicode Rendering Test File

This file contains text in multiple languages and formats.

---

# Code Blocks

Inline code: `printf("Hello, 世界\n");`

C:

```c
#include <stdio.h>

int main(void) {
	printf("Hello, 世界!\n");
	return 0;
}
```

Python:

```python
def greet():
    print("नमस्ते 🌍")
```

Rust:

```rust
fn main() {
	println!("こんにちは世界");
}
```

---

# Headings

# Heading Level 1

## Heading Level 2

### Heading Level 3

#### Heading Level 4

##### Heading Level 5

###### Heading Level 6

---

# Latin Scripts

English:
- Hello, World!
- The quick brown fox jumps over the lazy dog.
- Café naïve résumé coöperate façade.

French:
- Voilà un élève très âgé.

German:
- Grüße aus München. Straße, groß, weiß.

Spanish:
- ¡Hola! ¿Cómo estás?

Portuguese:
- São Paulo, ação, coração.

Polish:
- Zażółć gęślą jaźń.

Czech:
- Příliš žluťoučký kůň úpěl ďábelské ódy.

Turkish:
- Pijamalı hasta yağız şoföre çabucak güvendi.

---

# Indic Scripts

Hindi:
- नमस्ते, हैलो वर्ल्ड
- मेरा नाम अर्जुन है।
- क्या आप हिन्दी पढ़ सकते हैं?

Sanskrit:
- वसुधैव कुटुम्बकम्

Bengali:
- নমস্কার, পৃথিবী।

Tamil:
- வணக்கம் உலகம்

Telugu:
- హలో ప్రపంచం

Kannada:
- ನಮಸ್ಕಾರ ವಿಶ್ವ

Malayalam:
- ഹലോ ലോകമേ

Gujarati:
- હેલો વર્લ્ડ

Punjabi:
- ਸਤਿ ਸ੍ਰੀ ਅਕਾਲ

Urdu:
- ہیلو دنیا

---

# East Asian Scripts

Chinese (Simplified):
- 你好世界
- 今天天气很好。

Chinese (Traditional):
- 你好，世界。
- 學習永無止境。

Japanese:
- こんにちは世界
- 日本語の表示を確認します。
- 漢字、ひらがな、カタカナ

Japanese mixed:
- 東京2026オリンピック

Korean:
- 안녕하세요 세계
- 한글 렌더링 테스트

---

# Southeast Asian Scripts

Thai:
- สวัสดีชาวโลก

Vietnamese:
- Xin chào thế giới
- Tiếng Việt có dấu.

---

# Right-to-Left Scripts

Arabic:
- مرحباً بالعالم
- اللغة العربية تُكتب من اليمين إلى اليسار.

Hebrew:
- שלום עולם
- עברית נכתבת מימין לשמאל.

Mixed RTL/LTR:
- Version 2.1 إصدار
- English العربية English
- abc אבג 123

---

# Emoji

Faces:
😀 😁 😂 🤣 😃 😄 😅 😆 😉

People:
👩 👨 👩‍💻 👨‍🔬 👨🏾‍🚀

Animals:
🐶 🐱 🐼 🦊 🦄 🐧

Objects:
⌚ 📱 💻 🖨️ 🎧

Flags:
🇮🇳 🇺🇸 🇯🇵 🇫🇷 🇩🇪 🇨🇳

Complex Emoji:
👨‍👩‍👧‍👦
👩🏽‍💻
🏳️‍🌈
🏴‍☠️
👍🏿
❤️
❤️‍🔥

---

# Mathematical Symbols

Basic:
+ − × ÷ = ≠ ≈ ≤ ≥ ± ∞

Greek:
α β γ δ ε θ λ μ π σ Ω

Set Theory:
∈ ∉ ∪ ∩ ⊂ ⊆ ∅

Logic:
∀ ∃ ¬ ∧ ∨ ⇒ ⇔

Arrows:
← ↑ → ↓ ↔ ↕ ⇐ ⇒ ⇑ ⇓

Misc:
√ ∛ ∑ ∏ ∫ ∂ ∇

Equation:
f(x) = ∑ᵢ₌₀ⁿ xᵢ²

Superscripts/Subscripts:
x² y³ H₂O CO₂ E=mc²

---

# Various Indentation

No indentation.

    Four spaces indentation.

        Eight spaces indentation.

            Twelve spaces indentation.

- Level 1
    - Level 2
        - Level 3
            - Level 4

1. First
    1. Nested
        1. Deeply nested

---

# Bullet Lists

- Item one
- Item two
- Item three

* Asterisk item
* Another item

+ Plus item
+ Another plus item

---

# Numbered Lists

1. First item
2. Second item
3. Third item

1. Apples
2. Bananas
3. Oranges
   1. Small
   2. Large
      1. Extra Large

---

# Task List

- [ ] Incomplete task
- [x] Completed task
- [ ] Unicode rendering
- [x] Emoji rendering

---

# Definition List

Term 1
: Definition for term one.

Unicode
: A universal character encoding standard.

---

# Block Quotes

> This is a block quote.
>
> It spans multiple lines.

> Nested quote:
>
> > Second level.
> >
> > > Third level.

---

# Tables

| Language | Greeting | Script |
|----------|----------|--------|
| English | Hello | Latin |
| हिन्दी | नमस्ते | Devanagari |
| 中文 | 你好 | Han |
| 日本語 | こんにちは | Kana + Kanji |
| العربية | مرحباً | Arabic |

---

# Wide Character Table

| Char | Description |
|------|-------------|
| A | Latin |
| 中 | CJK |
| 😀 | Emoji |
| क | Devanagari |
| 🏳️‍🌈 | ZWJ Emoji |
| ة | Arabic |
| 한 | Hangul |

---

# Alignment Stress Test

| Left | Center | Right |
|:-----|:------:|------:|
| abc | 中 | 123 |
| 😀 | नमस्ते | مرحباً |
| short | very very long text | end |

---

# Long Lines

ThisIsAnExtremelyLongWordWithoutAnySpacesToTestWrappingBehaviorAndOverflowHandlingInTheEditorAndEnsureThatScrollingStillWorksCorrectly.

这是一个非常非常长而且没有空格的中文字符串用于测试编辑器的自动换行行为以及水平滚动功能是否正常工作。

---

# Combining Characters

é
ā
ñ
ö

Zalgo:
H̴̨̢̪̺̜̜͂̓̋͑͗̾͗͋e̶̟̥͔̰͂̍̿̈́̏̿̚l̶͎͖̯̇̽̐̈́̈́͗͝l̶̥̪̫̦͌͗͂̈́̓o̴͎͂̍͐̅̎͘̚͝

---

# Box Drawing

┌─────────────────────────┐
│ Unicode Box Drawing     │
├─────────────────────────┤
│ Light lines             │
│ Heavy lines             │
└─────────────────────────┘

╔═════════════════════════╗
║ Double Line Box         ║
╠═════════════════════════╣
║ Testing CP437 symbols   ║
╚═════════════════════════╝

---

# Braille

⠋⠕⠕ ⠃⠁⠗

---

# Miscellaneous Symbols

★ ☆ ✓ ✔ ✗ ✘
☀ ☁ ☂ ☃ ☕
♠ ♥ ♦ ♣
♫ ♪ ♬ ♩
⚠ ☢ ☣ ⚙

---

# End of File

The end. 🎉
