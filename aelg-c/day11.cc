
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <unistd.h>
#include <cstring>

using namespace std;

int const NUMBER_OF_ELEMENTS = 7;

struct State;
void print_state(State const &state);
struct State{
  int chips[NUMBER_OF_ELEMENTS];
  int generators[NUMBER_OF_ELEMENTS];
  int elevator;
  int cost;
  mutable int h;
  mutable uint64_t sort_value_;
  State():chips{0},generators{0},h(-1),sort_value_(0){};
  State(const State &s):State() {
    for(int i = 0; i < NUMBER_OF_ELEMENTS; ++i){
      chips[i] = s.chips[i];
      generators[i] = s.generators[i];
    }
    cost = s.cost;
    elevator = s.elevator;
  };
  void calc_h() const {
    h = 0;
    const size_t size = NUMBER_OF_ELEMENTS;
    for(size_t i = 0; i < size; ++i){
      h += 3 - chips[i];
      h += 3 - generators[i];
    }
    h = h*2;
    if(h<0) h = 0;
    h = cost + h;
  }
  bool operator==(State const &rhs) const {
    bool res = sort_value() == rhs.sort_value();
    sort_value_ = 0;
    rhs.sort_value_ = 0;
    return res;
  }
  bool operator<(State const &rhs) const {
    return sort_value() < rhs.sort_value();
  }
  uint64_t sort_value() const {
    if(sort_value_ == 0){
      size_t size = NUMBER_OF_ELEMENTS;
      sort_value_ = 0;
      bool invalid = false;
      int tmp_array[16] = {0};
      for(int i = 0; i < 16; ++i)tmp_array[i] = 0;
      for(size_t i = 0; i < size; ++i){
        int val = (chips[i]&3) | ((generators[i]&3)<<2);
        if(val <16){
          ++tmp_array[val];
        }
        else invalid = true;
      }
      for(int i = 0; i < 16; ++i) {
        sort_value_ |= tmp_array[i] << (3*i); 
      }
      if(elevator>0)
        sort_value_ = sort_value_ | (static_cast<int64_t>(elevator) << (3*16));
      if(invalid){
        sort_value_ = -1;
      }
    }
    return sort_value_;
  }
  size_t hash() const {
    return sort_value();
  }
};

struct StateHasher {
  size_t operator()(State const& s) const {return s.hash();}
};

struct Comp{
bool operator()(State const &lhs, State const &rhs) const {
  if (lhs.h == -1) lhs.calc_h();
  if (rhs.h == -1) rhs.calc_h();
  return lhs.h > rhs.h;
}
};

// A* not correct probably something to do with copy assignment?
//using p_queue = priority_queue<State, vector<State>, Comp>;
using p_queue = queue<State>;
using mySet = set<State>;
//using mySet = unordered_set<State, StateHasher>;


void add_if_valid(p_queue &q, mySet &s, State const &current){
  //print_state(current);
  if(current.elevator < 0) return;
  if(current.elevator > 3) return;
  const size_t size = NUMBER_OF_ELEMENTS;
  for(size_t i = 0; i < size; ++i){
    if(current.chips[i] != current.generators[i]){
      for(size_t j = 0; j < size; ++j){
        if(current.chips[i] == current.generators[j]) return;
      }
    }
  }
  if(s.count(current)) return;
  s.insert(current);
  q.push(current);
}

State generate_new_state(State const &current, int diff, int a, int b){
  int const size = static_cast<int>(NUMBER_OF_ELEMENTS);
  State new_state(current);
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
  //cout << new_state.sort_value_ << endl;
  return new_state;
}

void add_new_state(p_queue &q, mySet &s, State const &current){
  size_t const size = NUMBER_OF_ELEMENTS;
  for(size_t i = 0; i < size*2; ++i){
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
    for(size_t i = 0; i < NUMBER_OF_ELEMENTS; ++i){
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
  //* // Linus input, rätt svar 71
  initial.chips[0] = 1;
  initial.chips[2] = 1;
  /*/
  //* //Daniels input, rätt svart 61
  initial.chips[0] = 2;
  initial.generators[0] = 1;
  initial.chips[1] = 1;
  initial.generators[1] = 1;
  initial.chips[2] = 1;
  initial.generators[2] = 1;
  //*/
  initial.elevator = 0;
  initial.cost = 0;
  
  p_queue q; 

  q.push(initial);

  mySet s;
  s.insert(initial);

  State done;
  for (size_t i = 0; i < NUMBER_OF_ELEMENTS; ++i){
    done.generators[i]=3;
    done.chips[i] = 3;
  }
  done.elevator = 3;

  int count = 0;
  while(!q.empty()){
    ++count;
      State current(q.front());
      q.pop();
        
      if(! (current < done) && !(done < current)){
        break;
      }
      add_new_state(q, s, current);
  }

  if(!s.count(done)) cout << "Error!" << endl;
  cout << s.find(done)->cost << endl;

  return 0;
}
