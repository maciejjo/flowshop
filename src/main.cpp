#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

#include "Instance.h"

using namespace std;

int Job::last_id = 0;

int main() {

// Parametry                 
int liczba_przedzialow = 4;  
int liczba_zadan = 10;     
int max_czas_gotowosci = 5;
int liczba_przestojow = 3;
int max_czas_przestoju = 2;    
bool wyswietlanie = 1;       

  // Random seed
  srand(time(NULL));

  // Generowanie tablic przedziałów
  queue<Range> przedzialy;
  
//  for(int i = 0; i < liczba_przedzialow; i++ ) {
   // int max_val = pow(10,i+1);
   // przedzialy.push(Range(liczba_zadan / liczba_przedzialow, 1, max_val, 0, max_czas_gotowosci));

//    przedzialy.push(Range(liczba_zadan / liczba_przedzialow, l_dol, l_gora, 0, max_czas_gotowosci));
//    przedzialy.push(Range(liczba_zadan / liczba_przedzialow, p_dol, p_gora, 0, max_czas_gotowosci));
 przedzialy.push(Range(liczba_zadan / liczba_przedzialow, 1, 20, 10, max_czas_gotowosci));
 // }
  

  // Tworzenie instancji z zadanych przedziałów
  Instance *instancja = new Instance(&przedzialy, wyswietlanie, liczba_przestojow, liczba_zadan, max_czas_przestoju);

  // Wyświetlanie zadań włożonych do instancji
  if(wyswietlanie) {
    for(unsigned int i = 0; i < instancja->jobs.size(); i++)
      printf("(main) Wektor zadań w instancji: Zadanie %d [%d | %6d %6d %6d]\n",
          instancja->jobs[i].id, instancja->jobs[i].ready_time, instancja->jobs[i].operation_times[0],
          instancja->jobs[i].operation_times[1], instancja->jobs[i].operation_times[2]);
  }

  // Tworzenie maszyn
  Machine *maszyny = new Machine[3];

  // Algorytm losowy
//  instancja->RandomScheduling(maszyny);
  Instance instancja_random = *instancja;
  Instance instancja_sjf = *instancja;

  instancja_random.RandomScheduling(maszyny);
  instancja_sjf.ShortestJobScheduling(maszyny);


  // Sprzątanie
  delete[] maszyny;
  delete instancja;

  // Koniec

  return EXIT_SUCCESS;
}
