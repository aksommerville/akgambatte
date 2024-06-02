# akgambatte

Forked from Gambatte r571 by sinamas.

## TODO

- [x] Temporal frambuffer blending.
- - See game "Zas". There's a trick GB games used, toggling layers each frame, to create multiple background layers.
- - The original screen I guess its pixels took longer than one frame to fully change, so one could get away with this trick.
- - Can we simulate it by drawing each frame at <1 opacity?
- - It's going to be a change in emuhost most likely.
- - >>> Changing the final framebuffer transfer to 0.5 opacity fixes it pretty good for Zas.
- - Call it "pixel-refresh".
- - [x] Extract to a uniform in eh_render and allow config via command line. Default 1.
- - [x] Find other games impacted by this.
- - - King of Fighters 2003: Not exactly the same problem. 0.5 looks maybe better, maybe worse.
- - - Nothing else with the Faulty flag was this fault.
- - - I swear I've seen this behavior before, but can't seem to find any examples beyond Zas.
- - [x] What do lower opacities look like for games that don't need it? ...generally not a problem, but sometimes bad, and usually undesirable.
- - - Konami J2 Collection: Parodius: Looks great.
- - - Kirikou: Still gorgeous.
- - - Kid Niki: Better without but perfectly tolerable with.
- - - Robot Fight: Does no harm. Better without, imho.
- - - Yoshi's Cookie: No problem.
- - - Wario Land 3: Looks fine.
- - - Wizards Wrrs 10: Better without, noticeably. I think it's due to unrelated sprite-limit faults, but low pixel-refresh exacerbates it.
- - - Oracle of Seasons: Better without.
- - - The Castlevania Adventure: Much better without, due to exacerbated blink faults.
- - [x] Should we set 0.5 by default for all Gameboy? NO, do it per-game.
