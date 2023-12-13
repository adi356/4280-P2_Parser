# 4280 Project 2 Parser

## How to run

## Task

### BNF
<program> -> <vars> xopen <stats> xclose
<vars>    -> empty | xdata <varList>
<varList> -> identifier : integer ; | identifier : integer <varList>
<exp>     -> <M> / <exp> | <M> * <exp> | <M>
<M>       -> <N> + <M> | <N>
<N>       -> <R> - <N> | ~<N> | <R>
<R>       -> ( <exp> ) | identifier | integer
<stats>   -> <stat> <mStat>
<mStat>   -> empty | <stat> <mStat>
<stat>    -> <in> | <out> | <block> | <if> | <loop> | <assign>
<block>   -> {<vars> <stats>}
<in>      -> xin >> identifier;
<out>     -> xout << <exp>;
<if>      -> xcond [<exp> <RO> <exp>] <stat>
<loop>    -> xloop [<exp> <RO> <exp>] <stat>
<assign>  -> xlet identifier <exp>;
<RO>      -> <<(onetoken) | >> (one token) | < | >| = | %