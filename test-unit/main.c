#include "test.h"
#include <locale.h>

Libj *libj;

int main() {
    E(libj_start(&libj));
    assert(setlocale(LC_NUMERIC, "C"));
    sanity_check();
    assert(setlocale(LC_NUMERIC, "ru_RU.UTF-8"));
    sanity_check();
    E(libj_finish(&libj));
}
