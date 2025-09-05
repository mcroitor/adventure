# Adventure

A simple adventure game written in C++. A Player can move in the Square World, pick up items, fight with monsters, and level up.

## Game representation

- `Engine`: un obiect ce implementeaza logica jocului;
- `Player`: un obiect ce reprezinta jucatorul, cu atribute precum pozitia, viata, si echipamentul;
- `Item`: un obiect ce reprezinta un item in ce poate echipa jucatorul;
- `Direction`: un enum ce reprezinta directiile in care se poate misca jucatorul;
- `Monster`: clasa ce reprezinta un monstru cu atribute precum viata si puterea de atac;
- `Point`: clasa ce reprezinta o pozitie in harta;
- `Chest`: clasa ce reprezinta o comoara ce poate contine iteme;
- `Obstacle`: clasa ce reprezinta un obstacol in harta;
- `Renderer`: clasa ce se ocupa de afisarea jocului pe ecran, fi consola sau GUI;
- `Map`: clasa prezinta harta lumii.
