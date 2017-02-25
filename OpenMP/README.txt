Vijaeac Cristian-Octavian
335CB

TEMA1 - Game of Life

Pasi in elaborarea programului serial:
	1)am citit din fisier numarul de linii si de coloane
	2)am construit matricea cu stari initiale citind din fisier linie cu linie,ignorand spatiile goale
	3)am bordat matricea cu 4 margini in functie de regulile descrise in enunt
	4)am copiat aceasta matrice peste cea auxiliara
	5)pentru fiecare iteratie luam fiecare celula in parte si calculam numarul de vecini "alive" pe care ii avea aceasta
	6)dupa acest calcul,in functie de numarul obinut,pastram sau schimbam starea celulei
	7)aplicam acest procedeu pentru toate celulele
	8)la finalul unei iteratii,bordam matricea auxiliara in functie de noile valori obtinute si o copiam peste cea originala
	9)la finalul iteratiilor,scriem in fisier rezultatul

Pasi suplimentari in elaborarea programului paralel:
	2 paralelizari : 1 pentru prima copiere a solutiei initiale si a doua pentru fiecare copiere a matricii auxiliare peste cea originala in cadrul unei iteratii
	1 paralizare pentru prelucrarea matricii la fiecare pas(iteratie)

Probleme intampinate:
	-timp mai mare de rulare chiar si pe 8 threaduri fata de serial
	Solutie:variabila "no_alive" din cadrul for-urilor trebuia declarata private,altfel mai multe threaduri incercau accesarea ei si trebuiau sa astepte,lucru care 		marea ft mult timpul de executie
	-timp mai mare de rulare chiar si pe 8 threaduri fata de serial
	Solutie:cele 2 matrici trebuiau declarate shared

Posibila optimizare:
	Pentru o paralelizare mai buna,in loc de collapse(2) as fi putut face un singur for pentru parcurgerea matricii urmand ca determinarea liniei si a coloanei curente sa o fac din calcule bazate pe indicele for-ului de la 0 la nr_linii * nr_coloane.
