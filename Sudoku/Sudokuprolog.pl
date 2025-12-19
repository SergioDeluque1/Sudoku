/* sudokuproject.pl */
:- use_module(library(clpfd)).

solve(Rows) :- 
    length(Rows, 4), 
    maplist(same_length(Rows), Rows),
    append(Rows, Vs), Vs ins 1..4,
    maplist(all_distinct, Rows),
    transpose(Rows, Columns),
    maplist(all_distinct, Columns),
    Rows = [As,Bs,Cs,Ds],
    blocks(As, Bs),
    blocks(Cs, Ds),
    maplist(label, Rows),
    maplist(portray_clause, Rows).

blocks([], []).
blocks([N1, N2 | Ns1], [N3, N4 | Ns2]) :- 
    all_distinct([N1, N2, N3, N4]),
    blocks(Ns1, Ns2).