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

assert 0 "main(){return 0;}"
assert 42 "main(){return 42;}"
assert 21 "main(){return 5+20-4;}"
assert 41 "main(){return  12 + 34 - 5 ;}"
assert 17 "main(){return   5* 3+   6 /3  ;}"
assert 15 "main(){return   5* (3+   6  ) /3  ;}"
assert 47 "main(){return 5+6*7;}"
assert 15 "main(){return 5*(9-6);}"
assert 4 "main(){return (3+5)/2;}"
assert 1 "main(){return -5++10+-4;}"
assert 1 "main(){return 5-(3+   3)+   0-(-2);}"
assert 1 "main(){return 100==100;}"
assert 1 "main(){return 100!=50;}"
assert 0 "main(){return 100==(50+50   -0)*100;}"
assert 1 "main(){a=1;return a;}"
assert 11 "main(){a=1;b=c=5;d=a+b+c;return d;}"
assert 11 "main(){foo=1;buzz=c=5;d=foo+buzz+c;return d;}"
assert 2 "main(){a=1;b=1;if(a==b) c=a+b; return c;}"
assert 9 "main(){a=3;b=1;if(a==b) c=a+b;else d=3*a;return d;}"
assert 5 "main(){tanaka=0;while(tanaka<5) tanaka = tanaka+1;return tanaka;}"
assert 10 "main(){suzuki=0;for(tanaka=0;tanaka<5;tanaka = tanaka+1)suzuki=tanaka+suzuki;return suzuki;}"
assert 2 "main(){a=5;b=5;if(a==b){a=10;b=5;return a/b;}}"
assert 6 "main(){a=1;b=2;for(;;){if(a>b){return a*b;}a=a+1;}}"
assert 0 "foo(x, y){return 0;}main(){return foo(1, 3);}"
assert 4 "foo(x, y){return x+y;}main(){return foo(1, 3);}"
assert 32 'main() { return ret32(); } ret32() { return 32; }'
assert 7 'main() { return add2(3,4); } add2(x,y) { return x+y; }'
assert 1 'main() { return sub2(4,3); } sub2(x,y) { return x-y; }'
assert 55 'main() { return fib(9); } fib(x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

echo OK