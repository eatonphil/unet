#ifndef COMMON_H
#define COMMON_H

using namespace std;

typedef int error;

#define ok 0;

void fatal(string msg, error err) {
  cout << msg << endl;
  exit(err);
}

#endif
