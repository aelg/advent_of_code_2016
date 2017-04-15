
#include <iostream>
#include <vector>
#include <queue>
#include <set>

using namespace std;

int const NUMBER_OF_ELEMENTS = 14;

struct State{
  vector<int> chips;
  vector<int> generators;
  int elevator;
  int cost;
  bool operator==(State const &rhs) const {
    return (!(*this < rhs) && !(rhs < *this));
  }
  bool operator<(State const &rhs) const {
    for(size_t i = 0; i < chips.size(); ++i){
      if(chips[i] == rhs.chips[i]) continue;
      return chips[i] < rhs.chips[i];
    }
    for(size_t i = 0; i < generators.size(); ++i){
      if(generators[i] == rhs.generators[i]) continue;
      return generators[i] < rhs.generators[i];
    }
    return elevator < rhs.elevator;
  }
};

void print_state(State const &state);

void add_if_valid(queue<State> &q, set<State> &s, State const &current){
  //print_state(current);
  if(current.elevator < 0) return;
  if(current.elevator > 3) return;
  for(size_t i = 0; i < current.chips.size(); ++i){
    if(current.chips[i] != current.generators[i]){
      for(size_t j = 0; j < current.generators.size(); ++j){
        if(current.chips[i] == current.generators[j]) return;
      }
    }
  }
  if(s.count(current)) return;
  s.insert(current);
  q.push(current);
}

State generate_new_state(State const &current, int diff, int a, int b){
  int const size = static_cast<int>(current.chips.size());
  State new_state = current;
  int generator_a = -1;
  int generator_b = -1;
  int chip_a = -1;
  int chip_b = -1;
  if(a < size) generator_a = a;
  if(a >= size && a < size*2) chip_a = a-size;
  if(b < size) generator_b = b;
  if(b >= size && b < size*2) chip_b = b-size;
  if(generator_a != -1 && current.elevator == new_state.generators[generator_a])
    new_state.generators[generator_a] += diff;
  if(generator_b != -1 && current.elevator == new_state.generators[generator_b]) 
    new_state.generators[generator_b] += diff;
  if(chip_a != -1 && current.elevator == new_state.chips[chip_a])
    new_state.chips[chip_a] += diff;
  if(chip_b != -1 && current.elevator == new_state.chips[chip_b])
    new_state.chips[chip_b] += diff;
  if(new_state == current) new_state.elevator = -2;
  new_state.elevator += diff;
  if(a == b) new_state.elevator = -1;
  ++new_state.cost;
  return new_state;
}

void add_new_state(queue<State> &q, set<State> &s, State const &current){
  size_t const size = current.chips.size();
  for(size_t i = 0; i <= size*2; ++i){
    for(size_t j = i+1; j <= size*2; ++j){
      add_if_valid(q, s, generate_new_state(current, 1, i, j));
      add_if_valid(q, s, generate_new_state(current, -1, i, j));
    }
  }
}

void print_state(State const &state){
  for(int floor = 3; floor >= 0; --floor){
    if(state.elevator == floor) cout << "E ";
    else cout << ". ";
    for(size_t i = 0; i < state.chips.size(); ++i){
      if(state.generators[i] == floor) cout << "G ";
      else cout << ". ";
      if(state.chips[i] == floor) cout << "C ";
      else cout << ". ";
    }
    cout << endl;
  }
  cout << endl;
}

int main(){
  State initial;
  initial.generators = {0, 0, 0, 0, 0, 0, 0};
  initial.chips= {1, 0, 1, 0, 0, 0, 0};
  //initial.generators = {1,2};
  //initial.chips= {0, 0};
  initial.elevator = 0;
  initial.cost = 0;
  
  queue<State> q; 
  q.push(initial);

  set<State> s;
  s.insert(initial);

  State done;
  done.generators = {3, 3, 3, 3, 3};
  done.chips= {3, 3, 3, 3, 3};
  //done.generators = {3, 3};
  //done.chips= {3, 3};
  done.elevator = 3;

  while(!q.empty()){
      State current = q.front();
      q.pop();
      //if(! (current < done) && !(done < current)){
      //  break;
      //}
      add_new_state(q, s, current);
      //print_state(current);
  }

  if(!s.count(done)) cout << "Error!" << endl;
  cout << s.find(done)->cost << endl;

  return 0;
}
