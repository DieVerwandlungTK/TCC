#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./tcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 17 "   5* 3+   6 /3  ;"
assert 15 "   5* (3+   6  ) /3  ;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 1 "-5++10+-4;"
assert 1 "5-(3+   3)+   0-(-2);"
assert 1 "100==100;"
assert 1 "100!=50;"
assert 0 "100==(50+50   -0)*100;"
assert 1 "a=1;"
assert 11 "a=1;b=c=5;d=a+b+c;"
assert 11 "foo=1;buzz=c=5;d=foo+buzz+c;"
assert 2 "a=1;b=1;if(a==b) c=a+b;"
assert 9 "a=3;b=1;if(a==b) c=a+b;else d=3*a;"
assert 5 "tanaka=0;while(tanaka<5) tanaka = tanaka+1;"
assert 10 "suzuki=0;for(tanaka=0;tanaka<5;tanaka = tanaka+1)suzuki=tanaka+suzuki;return suzuki;"

echo OK