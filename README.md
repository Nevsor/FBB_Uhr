# FBB_Uhr
Eine Uhr als Theaterequisit, steuerbar über DMX

# Protokoll

| Kanal | Name | Werte |
|-------|------|-------|
| 1 | Position Stundenzeiger | 0-9: NOP <br> 10-249: 0-358.5° <br> 250-255: NOP |
| 2 | Geschwindigkeit Stundenzeiger | 0-255: Stillstand bis maximale Geschwindigkeit |
| 3 | Modus Stundenzeiger | 0-29 automatisch im Uhrzeigersinn <br> 30-59 NOP <br> 60-89 automatisch gegen Uhrzeigersinn <br> 90-119 NOP <br> 120-149 manuell im Uhrzeigersinn <br> 150-179 NOP <br> 180-209 manuell gegen Uhrzeigersinn <br> 210-255 NOP |
| 4 | Position Minutenzeiger | 0-9: NOP <br> 10-249: 0-358.5° <br> 250-255: NOP |
| 5 | Geschwindigkeit Minutenzeiger | 0-255: Stillstand bis maximale Geschwindigkeit |
| 6 | Modus Minutenzeiger | 0-29 automatisch im Uhrzeigersinn <br> 30-59 NOP <br> 60-89 automatisch gegen Uhrzeigersinn <br> 90-119 NOP <br> 120-149 manuell im Uhrzeigersinn <br> 150-179 NOP <br> 180-209 manuell gegen Uhrzeigersinn <br> 210-255 NOP |
| 7 | Synchronisationsmodus | 0-49: unabhängig <br> 50-99: synchron <br> 100-149: "Uhrmodus" <br> 150-189 NOP <br> 210-229 Nullpunkt setzen <br> 230-255 NOP |
