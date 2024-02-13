#include "test.h"
#include <locale.h>

Libj *libj;

int main() {
    E(libj_start(&libj));
    sanity_check();
    if (setlocale(LC_NUMERIC, "C")) {
        sanity_check();
    }
    if (setlocale(LC_NUMERIC, "ru_RU")) {
        sanity_check();
    }
    E(libj_finish(&libj));
}

