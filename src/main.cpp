#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

#include <queue>

#include "Range.h"
#include "Job.h"
#include "Instance.h"
#include "Machine.h"

#define MACHINES 3

using namespace std;

int Job::last_id = 0;

int main() {

  int liczba_przedzialow = 2;
  int liczba_zadan = 10;
  int max_czas_gotowosci = 10;

  srand(time(NULL));

  // Generowanie tablic przedziałów
  queue<Range> przedzialy;
  for(int i = 0; i < liczba_przedzialow; i++ ) {
    int max_val = pow(10,i+1);
    przedzialy.push(Range(liczba_zadan / liczba_przedzialow, 1, max_val, 0, max_czas_gotowosci));
  }

  // Tworzenie instancji z zadanych przedziałów
  Instance *instancja = new Instance(&przedzialy);

  // Wyświetlanie zadań włożonych do instancji
  for(unsigned int i = 0; i < instancja->jobs.size(); i++)
    printf("(main) Wektor zadań w instancji: Zadanie %d [%d | %6d %6d %6d]\n", instancja->jobs[i].id, instancja->jobs[i].ready_time, instancja->jobs[i].operation_times[0], instancja->jobs[i].operation_times[1], instancja->jobs[i].operation_times[2]);

  // Tworzenie maszyn
  //Machine *maszyny = new Machine[MACHINES];

  instancja->SortJobs();
  printf("Sortowanie...\n");

  for(unsigned int i = 0; i < instancja->jobs.size(); i++)
    printf("(main) Wektor zadań w instancji: Zadanie %d [%d | %6d %6d %6d]\n", instancja->jobs[i].id, instancja->jobs[i].ready_time, instancja->jobs[i].operation_times[0], instancja->jobs[i].operation_times[1], instancja->jobs[i].operation_times[2]);

  // Wyświetlanie gotowości zadań
  for(int i = 0; i <= max_czas_gotowosci; i++) {
    printf("(main) W momencie %d gotowe są zadania:", i);
    vector <Job> gotowe = instancja->GetReadyJobs(i);
    while(!gotowe.empty()) {
      printf(" %d", gotowe.back().id);
      gotowe.pop_back();
    }
    printf("\n");
  }

  return EXIT_SUCCESS;
}
