/**
 * Goran Futac
 * 0036431224
 * prevedeno sa gcc 4.5.2
 *
 * najkraci put izmedu dva najudaljenija vrha - izracunati najkrace putove izmedu svaka dva vrha, rezultat je onaj najveci
 * (najudaljnija dva vrha, put koji je pronaden je najkraci)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>

#define INF 0x7fffffff
#define UNDEF 0xef

using namespace std;

//komaprator koji se interno koristi kod sortiranja min heapa nakon dodavanja i skidanja elementa s vrha
template<typename a, typename b>
struct comparator{
	bool operator()(pair<a, b> &x, pair<a, b> &y){
		if (x.second > y.second) return true;
		if (y.second > x.second) return false;
		return x.first > y.first;
	}
};

//min heap
typedef priority_queue<pair<int, int>, vector<pair<int, int> >, comparator<int, int> > min_heap;


//pomocna funkcija koja radi glavni dio posla
void foo(int roditelj[], int vrh){
	if (roditelj[vrh] == UNDEF) return;
	vrh = roditelj[vrh];
	foo(roditelj, vrh);
	cout << vrh << " -> ";
}

//funkcija koja ispisuje put od vrha start do end
void put(int roditelj[], int start, int end){
	foo(roditelj, end);
	cout << end << endl;
}

/**
 * Trazi najkrai put u zadanom grafu i zadanog vrha do svih ostalih vrhova.
 * @param graf Matrica susjedstva grafa
 * @param brVrhova Broj vrhova grafa
 * @param start Pocetni vrh
 * @param ukupno Matrica udaljenosti (i-ti redak predstavlja najkrace udaljenosti i-tog vrha od svih ostalih)
 * @param roditelji Matrica roditelja (j-ti element i-tog sadrzi indeks roditelja vrha j ako se traze najkraci putevi od vrha i)
 */
void dijkstra(int **graf, int brVrhova, int start, int **ukupno, int **roditelji){
	//skup parova (indeks vrha, udaljenost do vrha), tj. (v, l(v)); na vrhu se uvijek nalazi onaj s namanjim l(v). heap koristen radi
	//najmanje vremenske slozenosti dodavanja i skidanja elemenata (O(log n))
	min_heap set;

	//lista udaljenosti - na k-tom mjestu pise udaljensot k-tog vrha od vrha start
	int *udaljenosti = new int[brVrhova];

	//lista roditelja - na k-tom mjestu pise indeks vrha roditelja vrha k
	int *roditelj = new int[brVrhova];

	//lista posjecenih vrhova (skup S)
	bool posjecen[brVrhova];

	//inicijalizacija
	for (int i = 0; i < brVrhova; ++i){
		udaljenosti[i] = INF;
		posjecen[i] = false;
	}

	udaljenosti[start] = 0;
	roditelj[start] = UNDEF;

	//pocetno, gledamo vrhove susjedne vrhu start i racunamo udaljenost do svakog (pocetno je udaljenost +beskonacno).
	//vrh "start" stavljamo u skup S (oznacavamo ga posjecenim).
	//sve susjedne vrhove (za koje smo izracunali novu udaljenost) stavljamo na heap - heap NIJE skup not(S), jer se na njemu mogu naci
	//dva para (v, l(v)) i (w, l(w)), v == w, l(v) != l(w).
	//skup not(S) cine svi neposjeceni vrhovi (ovdje - svi indeksi polja "posjecen" koji su false)
	set.push(pair<int, int> (start, udaljenosti[start]));
	while(!set.empty()){
		//za sve vrhove do kojih se moze doci iz vrhova skupa S odaberi onaj s najmanjim l(v)
		pair<int, int> v = set.top();
		set.pop();

		//s obzirom da se moze dogoditi da se na heapu nalazi isti vrh vise puta, ali s razlicitim l(v) (sto veci l(v) to je on blize
		//dnu), ako ga skinemo drugi (i svaki sljedeci) puta, sto znaci da je on vec posjecen, tj stavljen u skup S (a to znaci i da je
		//pronaden najkraci put do njega), ne uzimamo ga vise u obzir i idemo dalje (samo ga skinemo s heapa)
		if (posjecen[v.first]) continue;

		//a ako ga skidamo prvi put, to znaci da je do njega pronaden najkraci put i stavljamo ga u skup S
		posjecen[v.first] = true;

		//racunamo nove cijene puta do svakog od susjednih vrhova kao
		//l(u) = l(v) + dist(u, v)
		for (int u = 0; u < brVrhova; ++u){
			//ako se u vec nalazi u S, preskacemo ga
			if (posjecen[u]) continue;
			//s obzirom da provjeru obavljamo setnjoj po redcima matrice, na dijagonali su uvijek nule (jer je udaljenost
			//vrha do sebe samog jednaka nuli), preskacemo vrh na mjestu graf[i][i] za svaki i
			if (u == v.first) continue;
			//ili ako je vrh v nije povezan bridom s vrhom u (u matrici 0 na tom mjestu), preskacemo ga
			if (graf[v.first][u] == 0) continue;

			//inace racunamo novu udaljenost do vrha u
			int l = udaljenosti[v.first] + graf[v.first][u];
			//i ako je ona manja od stare
			if (l < udaljenosti[u]){
				//azuriramo ju
				udaljenosti[u] = l;
				//oznacavmo da je je v roditelj od u
				roditelj[u] = v.first;
				//na heap stavljamo novo stanje, par (vrh, udaljenost)
				set.push(pair<int, int> (u, udaljenosti[u]));
			}

		}
	}

	//opcenito, listu udaljenosti za svaki vrh u stavljamo kao i-ti redak matrice svih udaljenosti
	ukupno[start] = udaljenosti;
	//isto i za roditelje...
	roditelji[start] = roditelj;
}
//end dijkstra


int main(int argc, char **argv) {
	ifstream file;
	file.open(argv[1]);
	string linija;

	int brVrhova;
	getline(file, linija);
	istringstream iss(linija);
	iss >> brVrhova;
	getline(file, linija);

	//alokacija prostora za matrice
	//za graf od 500 vrhova potrebno oko 3mb prostora
	int **graf = new int*[brVrhova];
	for (int i = 0; i < brVrhova; ++i) graf[i] = new int[brVrhova];
	int **najkraciPutevi = new int*[brVrhova];
	int **roditelji = new int*[brVrhova];

	//ucitavanje grafa iz datoteke predane kao argument iz komandne linije
	int i = 0;
	while(!file.eof()){
		getline(file, linija);
		iss.clear();
		iss.str(linija);
		int j = 0;
		do {
			iss >> graf[i][j++];
		} while (iss);

		++i;
	}

	//za svaki vrh racuna najkrace puteve do svih ostalih, popunjava matricu najkracih puteva i roditelja
	for (int i = 0; i < brVrhova; ++i){
		dijkstra(graf, brVrhova, i,  najkraciPutevi, roditelji);
	}

	//u konacnoj matrici pronalazi najveci element (najveca najkraca udaljenost izmedu neka dva vrha) - rezultat izvodenja programa
	int max = 0;
	int x = 0, y = 0;
	for (int i = 0; i < brVrhova; ++i){
		for (int j = 0; j < brVrhova; ++j){
			if (najkraciPutevi[i][j] > max){
				max = najkraciPutevi[i][j];
				x = i;
				y = j;
			}
		}
	}

	//ispisuje rezultat
	cout << najkraciPutevi[x][y] << endl;
	//funkcija koja ispisuje put izmedu vrha x i y (oni su vrhovi cija je najkraca udaljenost najveca)
//	put(roditelji[x], x, y);

	//ciscenja smeca...
	for (int i = 0; i < brVrhova; ++i) {
		delete [] graf[i];
		delete [] najkraciPutevi[i];
		delete [] roditelji[i];
	}
	delete [] graf;
	delete [] najkraciPutevi;
	delete [] roditelji;

	return 0;
}
