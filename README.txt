Some of the ideas used:

1. The board class was changed to use two 64-bit integers instead of two bitsets. 

2. An evaluation function was created that looks at mobility and some positional factors (like "corners are good" and "X squares are bad"). Also, a hash table was generated giving every edge configuration a value. Since the evaluation function has to run so many times, it was kept on the simple side so that it is as fast as possible. 

3. The finding legal moves code was optimized as much as I could manage (I wish I knew how to write assembly for this), since this program computes the legal moves available an absurdly large amount of times.

4. A typical recursive minimax search was first implemented, and later alpha-beta pruning was added. Furthermore, prob cut is being implemented, though I'm not sure it will be completely ready in time (some standard deviations need to be calculated or estimated well). 

5. Time management is a bit of an issue. Iterative deepening was not used, and the program runs in depth 10 very comfortably, but might timeout in depth 11. The endgame search, however, is written to abort itself if it has to calculate too many nodes, and aborts very quickly. Then the typical minimax search to a certain depth is the fallback.

6. A couple of large hash tables are used. One is used by alpha-beta pruning to use past results to guess what the best move is going to be, so that the minimax algorithm can consider that one first. Another is used in the endgame so that once the program solves the game, it no longer has to resolve it later. (Actually, this would take up too much memory, so the program only keeps track of what a few of the moves should be, until it gets to a point in the game that it can very quickly resolve the game.)

7. More on iterative deepening not being used: theoretically I've heard that running lower depths first can improve the speed at the higher depths in alpha-beta pruning, if you keep track of certain things. This did not work for me, although I never went through the trouble of ordering all the possible moves.

This has been a lot of fun! I wish I could work on it for a lot longer.
