#include "test.h"

Libj *libj;

int main() {
    E(libj_start(&libj));
    sanity_check();
    E(libj_finish(&libj));
}
