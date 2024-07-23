# Aktatáskás KotH vezérlés

Az alábbi dokumentum az aktatáskás "A Hegy Királya" (King of the Hill) játékelem vezérlési instrukcióit tartalmazza.

## Játékleírás
A játék egy egyszerű KotH formátumot valósít meg, mely aktatáska formályában könnyen hordozható, így egy teljesen mobilis és tetszőleges elhelyezést tud biztosítani.

A cél a két színnel jelzett csapat számára az aktatáska "birtoklása", azaz a saját színükhöz tartozó gomb lenyomása. Ezt adott időkereten belül tetszőlegesen sokszor meg lehet tenni, a táska az éppen aktív szín számára fog másodpercenkét pontokat elszámolni.

A játékot az a szín nyeri, melynek leállításkor több pontja gyűlt össze.

## A rendszer állapotai
A táskában található mikrovezérlő tápellátásának biztosítása után a játék alapállapotba helyezi magát, melyet az aktív játék, a kiértékelés és az alapállapotba visszahelyezés lehetőségei követnek.

A játék bármely pillanatban az alábbi állapotok pontosan egyikét veszi fel:

- Alapállapot: a játék készenlétben áll, várja, hogy bármely gomb lenyomásra kerüljön. A reset, piros és sárga gombok együttesen folytonosan villognak.
- Sárga foglal: a játék a sárga szín számára számol el pontokat, a sárga gomb folytonosan világít.
- Piros foglal: a játék a piros szín számára számol el pontokat, a piros gomb folytonosan világít.
- Kiértékelés: a játék nem számol pontokat, a reset gomb villog ~~, a jelenlegi állás alapján nyerésre álló csapat gombja 1:1 ütemben villog, a vesztésre álló csapat gombja 1:N ütemben villog (ahol N a szín pontszám-összegbeli hátrányával arányos, legalább 2)~~.

## Állapotátmenetek
- Alapállapot -> Sárga foglal: a sárga gomb lenyomására.
- Alapállapot -> Piros foglal: a piros gomb lenyomására.
- Sárga foglal -> Piros foglal: a sárga gomb lenyomására.
- Piros foglal -> Sárga foglal: a piros gomb lenyomására.
- Sárga foglal -> Kiértékelés: a "Reset" gomb lenyomására.
- Piros foglal -> Kiértékelés: a "Reset" gomb lenyomására.
- Kiértékelés -> Sárga foglal: a sárga gomb lenyomására.
- Kiértékelés -> Piros foglal: a piros gomb lenyomására.
- Kiértékelés -> Alapállapot: a "Reset" gomb 5 másodpercig nyomvatartására.
[![Vezérlés állapotdiagram](https://app.eraser.io/workspace/Q2RTol8HHzs7mM4h7Qwu/preview?elements=HaKp08YZvj8B6MHMhmREGg&type=embed)](https://app.eraser.io/workspace/Q2RTol8HHzs7mM4h7Qwu?elements=HaKp08YZvj8B6MHMhmREGg)

## Kiegészítő funkciók
A játék egyszerűbb nyomonkövetésére kettő "luxusperiféria" van beépítésre szánva a táskába:

- [X] Egy 2x16 karakteres LCD kijelző, mely a mindenkori játékállapotot hivatott nyilvántartani, így a két szín pontos pontszámösszege külön-külön meg is tekinthető.
- [ ] Egy WiFi modul, melyre felcsatlakozva a táska állapotain túl a mikrovezérlő egyéb futás közbeni paraméterei is nyomonkövethetőek.
