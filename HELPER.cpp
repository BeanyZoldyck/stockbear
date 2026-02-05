#include "ofApp.h"
#include "ofMain.h"
#include "stockbear.h"
#include <cstdlib>
#include <math.h>  


const int None = 0;
const int Pawn = 1;
const int Knight = 2;
const int Bishop = 3;
const int Rook = 4;
const int Queen = 5;
const int King = 6;
const int White = 16;
const int Black = 8;
string PieceDictString = "xpnbrqk";
string PieceDicUpper = "XPNBRQK";

extern const int n = 8;

ofSoundPlayer soundPlayer;

void playSound(string path){
    soundPlayer.load(path);
    soundPlayer.play();
}
bool onScreen(ofRectangle rect){
	int right = rect.x + rect.width;
	int bottom = rect.y + rect.height;
    return  !(right < 0) && !(rect.x > WINLEN) && !(bottom < 0) && !(rect.y > WINHYT);
}

bool touchingCircles(int x1, int y1, int r1, int x2, int y2, int r2)
{
    int dX = x2 - x1;
    int dY = y2 - y1;
    return dX*dX + dY*dY < pow(r1+r2, 2); //true if touching
}
bool inArr(int n, int arr[], int x)
{
    for (int i = 0; i < n; i++)
    {
		if (arr[i] == x) return true;
	}
	return false;
}
int random(int min, int max)
{
    return min + rand() % (max - min + 1);
}

float random(int precision)
{
    int max = pow(10, precision);
    return (float)random(0,max)/max;
}

string boardToVec(int board[8][8], int side, string castling, string enPassant, int moveIdleness, int move)
{
    stringstream fen;
    // Convert the pieces on the board to FEN
    for (int rank = 0; rank < n; rank++) {
        int emptyCount = 0;
        for (int file = 0; file < n; ++file) {
            int piece = board[rank][file];
            if (piece == None) {
                emptyCount++;
            }
            else {
                if (emptyCount > 0) {
                    fen << emptyCount;
                    emptyCount = 0;
                }
                piece > White ? fen << PieceDicUpper[piece - White] : fen << PieceDictString[piece-Black];
            }
        }
        if (emptyCount > 0) {
            fen << emptyCount;
        }
        if (rank < n-1) {
            fen << '/';
        }
    }
    
    // Side to move 
    side == White ? fen << " w " : fen << " b ";

    // Castling rights 
    fen << castling;

    // En passant square 
    fen << " ";
    fen << enPassant;
    fen << " ";
    // Halfmove clock 
    fen << to_string(moveIdleness);
    fen << " ";
    // Fullmove number 
    fen << to_string(move);
    //cout << fen.str() << endl;
    return fen.str();
}

float ReLU(float x) { return max(float(0), x); };
float appraiseAI(int board[][8])
{
    float inp[65];
    for (int i = 0; i < 64; i++)
    {
        inp[i] = (float)board[i / 8][i % 8];
    }
    inp[64] = float(0);
    float h1neurons[100];

    for (int i = 0; i < 100; i++)
    {
        h1neurons[i] = 0;
        for (int j = 0; j < 65; j++)
        {
            h1neurons[i] += inp[j] * i_h1[j][i];
        }
        h1neurons[i] += h1B[i];
        h1neurons[i] = ReLU(h1neurons[i]);
    }
    float h2neurons[32];
    for (int i = 0; i < 32; i++)
    {
        h2neurons[i] = 0;
        for (int j = 0; j < 100; j++)
        {
            h2neurons[i] += h1neurons[j] * h1_h2[j][i];
        }
        h2neurons[i] += h2B[i];
        h2neurons[i] = ReLU(h2neurons[i]);

    }
    float output = 0;
    for (int i = 0; i < 32; i++)
    {
        output += h2neurons[i] * h2_o[i];
    }
    output += outBias;
    return output;
}
void removeDuplicates(int* n, int arr[])
{
    int len = *n;
	int newArr[64];
	int newArrLen = 0;
    for (int i = 0; i < len; i++)
    {
		bool duplicate = false;
        for (int j = 0; j < newArrLen; j++)
        {
			if (arr[i] == newArr[j]) duplicate = true;
		}
        if (!duplicate)
        {
			newArr[newArrLen] = arr[i];
			newArrLen++;
		}
	}
    for (int i = 0; i < newArrLen; i++)
    {
		arr[i] = newArr[i];
	}
	*n = newArrLen;

}
int minimum(int n, float tings[])
{
    int min = INT_MAX;
    int inde = 0;
    for (int i = 0; i < n; i++)
    {
        if (tings[i] < min) {
            min = tings[i]; inde = i;
        }
    }
    return inde;
}
char indexToRank(int index)
{
	return '8' - index / 8;
}
char indexToFile(int index)
{
	return 'a' + index % 8;
}

int otherTeam(int team)
{
    return White - (team - Black);
}

int piecesTeam(int piece)
{
    
	return piece > White ? White : piece == None ? None : Black; //alright maybe c++ is kinda based
}
int pieceType(int piece)
{
    return piece - piecesTeam(piece);
}
int pieceTypeEnemy(int piece)
{
    return piece - otherTeam(piecesTeam(piece));
}
int findChar(string str, char c)
{
    for (int i = 0; i < str.length(); i++)
    {
		if (str[i] == c) return i;
	}
	return -1;
}
int charToPeece(char C)
{
    int piece = 0;
    //int side = isupper(c)*8 + 8; ffaster, cooler version
    int side;
    isupper(C) ? side = White : side = Black; //more readable version
    piece += side;
    char c = tolower(C);
    switch (c)
    {
    case 'r':
        piece += Rook;
        break;
    case 'p':
        piece += Pawn;
        break;
    case 'n':
        piece += Knight;
        break;
    case 'b':
        piece += Bishop;
        break;
    case 'q':
        piece += Queen;
        break;
    case 'k':
        piece += King;
        break;
    }
    return piece;
}
int pieceToChar(int piece)
{
	int side = piecesTeam(piece);
	int type = pieceType(piece);
	char c = PieceDictString[type];
	if (side == White) c = toupper(c);
	return c;
}
const int spaces_until_info = 2;//for back when I supported fen....
const int startPieces[n] = { Rook,Knight,Bishop,Queen,King,Bishop,Knight,Rook };
class Board
{

    string CASTLING;
    int player;
    int MOVES;
    int HALFMVES;
    int state;
    int FORCED; //for en passant, index of board from h8 to a1, can only be one value at a time, so using int representation of index
    int tempBoard[8][8]; //for check checking
    bool kingMoved[2] = { false,false };
    bool rookMoved[4] = { false,false,false,false };
public:
    int board[n][n];
    int promoFlag;
    int lastMoveIndex;
    int dangerSquaresLen = 0;
    int legalLen = 0;
    int checkSq = -1;
    int dangerSquares[128]; // larger just incase too many duplicate threatenened squares caused segmentation fault
    int legalSquares[128];
    Board()
    {
        setBoard();
    }
    void updateTempRep()
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                tempBoard[i][j] = board[i][j];
            }
        }
    }
    void swapSides()
    {
		player = otherTeam(player);
	}
    void printBoard(int x[n][n])
    {
        for (int j = 0; j < n; j++)
        {
            for (int i = 0; i < n; i++)
            {
                if (inArr(dangerSquaresLen, dangerSquares, j* n + i)) printf("%3dx", x[j][i]);
                else printf("%4d", x[j][i]);
            }
            cout << endl;
        }
        cout << endl;
    }
    int checkSquared(int bord[8][8])
    {
       for (int i = 0; i < dangerSquaresLen; i++)
       {
		    if (bord[dangerSquares[i] / 8][dangerSquares[i] % 8] == King + player) return dangerSquares[i];
	   }
		return -1;
    }
    void setBoard()
    {
        promoFlag = -1;
        CASTLING = "KQkq";
        player = White;
        MOVES = 1;
        HALFMVES = 0;
        state = 5;
        FORCED = -1;
        dangerSquaresLen = 0;
        checkSq = -1;
        legalLen = 0;
        for (int i = 0; i < 4; i++)
        {
            kingMoved[i / 4] = false;
            rookMoved[i] = false;
        }
        for (int i = 0; i < 8; i++)
        {
            board[1][i] = Pawn + Black;
            board[6][i] = Pawn + White;
        }
        for (int i = 0; i < 8; i++)
        {
			board[0][i] = startPieces[i] + Black;
			board[7][i] = startPieces[i] + White;
		}
        for (int i = 2; i < 6; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                board[i][j] = None;
            }
        }
        updateTempRep();
    }
    void updateTheoreticalCheck(int destIndex, int selectIndex, int piece)
    {
        //updates checkSq to the value it would be if the move was made
        tempBoard[destIndex / 8][destIndex % 8] = piece;//move piece
        tempBoard[selectIndex / 8][selectIndex % 8] = None;//remove piece from old square
        player = otherTeam(player);//switches sides
        possSquares(tempBoard);
        player = otherTeam(player);
        checkSq = checkSquared(tempBoard);
        player = otherTeam(player);
    }
    bool pinned(int destIndex, int selectIndex, int piece)
    {
        //return true if move would expose king to danger
        int okabeSq = -1; //hypothetical checkSq
        int dRank = (destIndex/8) - (selectIndex/8);
        bool enPassant = pieceType(piece) == Pawn && piecesTeam(tempBoard[(destIndex / 8) - dRank][destIndex % 8]) == otherTeam(player);
        if (enPassant) //only true when capturing en passant
        {
            tempBoard[(destIndex / 8) - dRank][destIndex % 8] = None;//technically anything can be captured enpassant, but only pawns are able to change EP info
        }
        tempBoard[destIndex / 8][destIndex % 8] = piece;//move piece
        tempBoard[selectIndex / 8][selectIndex % 8] = None;//remove piece from old square
        //printBoard(tempBoard);
        player = otherTeam(player);//switches sides
        possSquares(tempBoard);
        player = otherTeam(player);
        okabeSq = checkSquared(tempBoard);
        if (enPassant && okabeSq != -1) //if move exposes king and was en passant, undo enpassant
        {
            board[(destIndex / 8) - dRank][destIndex % 8] = Pawn + otherTeam(player);//the rebooted one
        }
        updateTempRep();
        return okabeSq == -1 ? false : true;
    }
    void move(int * selectIndex, int * destIndex)
    {
        //if (checkSq != -1) return;
        updateTempRep();
        if (promoFlag > Black) return; //
        if (*destIndex == -1 || *selectIndex == -1) return;//if no destination or source, return
        int piece = board[*selectIndex / 8][*selectIndex % 8];//piece at selected square
        if (piece == None) { *destIndex = -1; return; } //if no piece at selected square, return
        if (player == None) return;//if not player's turn, return (changed to NONE because I'm not implementing side check, if you can move in the middle of the opponent's turn, you got it)
        if (*destIndex != FORCED) FORCED = -1;//if not en passant, reset forced
        bool legalMove = legal(board, piece, player, *selectIndex, *destIndex);
        if (!legalMove)
        {
            *destIndex = -1;
            *selectIndex = -1;
            return;//if not legal move, return
        }
        bool pinn = pinned(*destIndex, *selectIndex, piece);
        if (pinn)
        {
			*destIndex = -1;
			*selectIndex = -1;
			return;//if not pinned, return
		}
        if (checkSq == -1)//if legal move`
        {
            if (pieceType(piece) == Pawn || pieceType(board[*destIndex / 8][*destIndex % 8]) != None) HALFMVES = 0;//if pawn moves or capture, reset halfmove clock
			else HALFMVES++; //if you dont capture, increment halfmove clock
            board[*destIndex / 8][*destIndex % 8] = piece;//move piece
            board[*selectIndex / 8][*selectIndex % 8] = None;//remove piece from old square
            if (piece == Pawn + player && (*destIndex / 8) % 7 == 0) promoFlag = 2;//if pawn reaches end, promote to queen
            if (pieceType(piece) == Rook)
            {
                int srcFile = *selectIndex % 8;
                if (srcFile == 0 && player == Black) rookMoved[0] = true;
                else if (srcFile == 7 && player == Black) rookMoved[1] = true;
                else if (srcFile == 0 && player == White) rookMoved[2] = true;
                else if (srcFile == 7 && player == White) rookMoved[3] = true;
            }
            if (pieceType(piece) == King)
            {
				if (player == White) kingMoved[0] = true;
				else kingMoved[1] = true;
			}
            //pinn();
            possSquares(board);//update possible square the opp hits, TODO: overload to show legal move for selected piece
            //checkSq = checkSquared(true);
            player = otherTeam(player);//switches sides
            lastMoveIndex = *destIndex;
            if (player == Black) MOVES++;
            updateTempRep();
            
		}
        else//if legal move`
        {
            updateTheoreticalCheck(*destIndex, *selectIndex, piece);  //TODO: USE pinned HERE   
            if (checkSq == -1)
            {
                player = otherTeam(player);
                move(selectIndex, destIndex);
            }
            else
            {
                updateTempRep();
                player = otherTeam(player);
            }
        }
        *selectIndex = -1;
        *destIndex = -1;
       // printBoard(tempBoard);
    }
    void moveAIa(int side)
    {
        legalSquare(board, true);        
    }
    void promote()
    {
        board[lastMoveIndex / 8][lastMoveIndex % 8] = promoFlag;
        promoFlag = -1;
        
    }
    string owariDa()
    {
        if (state != 5)
        {
            string message;
            if (state == 0) message = player == White ? "CHECKMATE: BLACK WINS" : "CHECKMATE: WHITE WINS";
            else if (state == 1) message = "STALEMATE";
            else if (state == 2) message = "DRAW: INSUFFICIENT MATERIAL";
            else if (state == 3) message = "DRAW: FIFTY MOVE RULE";
            else if (state == 4) message = "DRAW: THREEFOLD REPETITION";
            return message;
        }
        return "";
    }
    void setState(int _state)
    {
		state = _state;
	}

    int getPlayer()
    {
        return player;
    }
    int getHalfMoves()
    {
        return HALFMVES;
    }
    int getMoves()
    {
		return MOVES;
	}
    void legalSquare(int bord[8][8], bool ai = false)
    {
        legalLen = 0;
        int slct[64];
        float appraisals[64];
        int slcts = 0;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (piecesTeam(bord[i][j]) != player) continue;
                for (int k = 0; k < 64; k++)
                {
                    if (pieceType(bord[i][j]) != Pawn && pieceType(bord[i][j]) != King && legal(bord, bord[i][j], player, 8 * i + j, k) && !pinned(k, 8 * i + j, bord[i][j]))//FASTER METHOD: create movement mask for each piece, and check which of those squares are legal
                    {
                        legalSquares[legalLen] = k;
                        slct[slcts] = 8 * i + j;
                        tempBoard[k / 8][k % 8] = bord[i][j];
                        tempBoard[i][i] = None;
                        appraisals[slcts] = appraiseAI(tempBoard); //make move n tepboard and store thoughts
                        slcts++;
                        legalLen++;
                    }
                    if (pieceType(bord[i][j]) == King)
                    {
                        int dstRank = k / 8;
                        int dstFile = k % 8;
                        int opp = bord[dstRank][dstFile];

                        if (piecesTeam(opp) == player) continue;
                        if (legalKing(bord, player, i, j, dstRank, dstFile, false) && !pinned(k, 8 * i + j, bord[i][j]))//FASTER METHOD: create movement mask for each piece, and check which of those squares are legal
                        {
                            legalSquares[legalLen] = k;
                            slct[slcts] = 8 * i + j;
                            tempBoard[k / 8][k % 8] = bord[i][j];
                            tempBoard[i][i] = None;
                            appraisals[slcts] = appraiseAI(tempBoard); //make move n tepboard and store thoughts
                            slcts++;
                            legalLen++;
                        }

                    }
                    if (pieceType(bord[i][j]) == Pawn)
                    {
                        int dstRank = k / 8;
                        int dstFile = k % 8;
                        int opp = bord[dstRank][dstFile];

                        if (piecesTeam(opp) == player) continue;
                        if (legalPawn(bord, player, i, j, dstRank, dstFile, false) && !pinned(k, 8 * i + j, bord[i][j]))//FASTER METHOD: create movement mask for each piece, and check which of those squares are legal
                        {
							legalSquares[legalLen] = k;
                            slct[slcts] = 8 * i + j;
                            tempBoard[k / 8][k % 8] = bord[i][j]; //i aint implementing en passant
                            tempBoard[i][i] = None;
                            appraisals[slcts] = appraiseAI(tempBoard); //make move n tepboard and store thoughts
                            slcts++;
							legalLen++;
						}

                    }
                }
                updateTempRep();
            }
        }
        if (ai)
        {
            int BestMoveQuotes = minimum(slcts, appraisals); //index of "best move"
            int slc = slct[BestMoveQuotes];
            int dst = legalSquares[BestMoveQuotes];
			move(&slc, &dst);
		}
        removeDuplicates(&legalLen, legalSquares);
        //printBoard(tempBoard);
    }

    void possSquares(int bord[8][8])
    {
        dangerSquaresLen = 0;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (piecesTeam(bord[i][j]) != player) continue;
                for (int k = 0; k < 64; k++)
                {
                    if (pieceType(bord[i][j]) != Pawn && legal(bord, bord[i][j], player, 8 * i + j, k))//FASTER METHOD: create movement mask for each piece, and check which of those squares are legal
                    {
                        dangerSquares[dangerSquaresLen] = k;
                        dangerSquaresLen++;
                    }
                    if (pieceType(bord[i][j]) == Pawn)
                    {
                        //bool stanninOnBinnis = piecesTeam(board[k / 8][k % 8]) == otherTeam(player);
                        //if (!stanninOnBinnis) continue;
                        bool offByOne = (k % 8 == j + 1 || k % 8 == j - 1);
                        if (!offByOne) continue;
                        bool upOrDownByOne = ((player == White && k / 8 == i - 1) || (player == Black && k / 8 == i + 1));
                        if (!upOrDownByOne) continue;
                        dangerSquares[dangerSquaresLen] = k;
                        dangerSquaresLen++;
                    }
                }

            }
        }
        removeDuplicates(&dangerSquaresLen, dangerSquares);
    }
private:
    
    void removeCastlingRight(char c)
    {
        string newCastling = "";
        for (int i = 0; i < CASTLING.length(); i++)
        {
            if (CASTLING[i] != c) newCastling += CASTLING[i];
        }
        CASTLING = newCastling;
    }
    bool legal(int board[8][8], int piece, int side, int src, int dst)
    {
        //return false;
        piece = piece - side;
        bool movingWhite = board[src / 8][src % 8] > White;
        if ((side == White && !movingWhite) || (side == Black && movingWhite)) { cout << "not ur turn" << endl; return false; }
        int dstRank = dst / 8;
        int dstFile = dst % 8;
        int opp = board[dstRank][dstFile];
        bool backDoor = ((side==White)&&(opp>White)) || ((side==Black)&&(opp>Black)&&(opp<White));
        if (backDoor) return false;
        int srcRank = src / 8;
        int srcFile = src % 8;
        if (piece == Pawn) return legalPawn(board, side, srcRank, srcFile, dstRank, dstFile);
        if (piece == Knight) return legalKnight(board, side, srcRank, srcFile, dstRank, dstFile);
        if (piece == Bishop) return legalBishop(board, side, srcRank, srcFile, dstRank, dstFile);
        if (piece == Rook) return legalRook(board, side, srcRank, srcFile, dstRank, dstFile);
        if (piece == Queen) return legalQueen(board, side, srcRank, srcFile, dstRank, dstFile);
        if (piece == King) return legalKing(board, side, srcRank, srcFile, dstRank, dstFile);
    }
    bool legalPawn(int board[8][8], int side, int srcRank, int srcFile, int dstRank, int dstFile, bool unironicallyMoving = true)
    {
        int dRank = dstRank - srcRank;
		int dFile = dstFile - srcFile;
		int opp = board[dstRank][dstFile];
        if (side == White)
        {
			if (dRank == -1 && dFile == 0 && opp == None) return true;
			if (dRank == -1 && abs(dFile) == 1 && opp > Black && opp < White) return true;
            if (dRank == -2 && dFile == 0 && srcRank == 6 && opp == None && board[5][srcFile] == None) {
                if (unironicallyMoving) FORCED = 8*(dstRank+1) + dstFile;
                return true;
            }
		}
        if (side == Black)
        {
			if (dRank == 1 && dFile == 0 && opp == None) return true;
			if (dRank == 1 && abs(dFile) == 1 && opp > White) return true;
			if (dRank == 2 && dFile == 0 && srcRank == 1 && opp == None && board[2][srcFile] == None) {
                if (unironicallyMoving) FORCED = 8 * (dstRank - 1) + dstFile;
                return true;
            }
		}
		if (abs(dRank) == 1 && abs(dFile) == 1 && dstRank == FORCED / 8 && dstFile == FORCED % 8) 
        {
            board[(FORCED / 8)-dRank][FORCED % 8] = None;//technically anything can be captured enpassant, but only pawns are able to change EP info
            FORCED = -1;
            return true;
        }
		return false;
	}
    bool legalKnight(int board[8][8], int side, int srcRank, int srcFile, int dstRank, int dstFile)
    {
        int dRank = dstRank - srcRank;
        int dFile = dstFile - srcFile;
        if (abs(dRank) == 2 && abs(dFile) == 1) return true;
        if (abs(dRank) == 1 && abs(dFile) == 2) return true;
        return false;
    }
    bool legalBishop(int board[8][8], int side, int srcRank, int srcFile, int dstRank, int dstFile)
    {
		int xDir = dstFile - srcFile;
        int yDir = dstRank - srcRank;
        if (abs(xDir) != abs(yDir)) return false;//diagonal check
        int xsign = xDir / abs(xDir);
        int ysign = yDir / abs(yDir);
        for (int i = 1; i < abs(xDir); i++)
        {
			int x = srcFile + i * xsign;
			int y = srcRank + i * ysign;
			if (board[y][x] != None) return false;
		}
        return true;
	}
    bool legalRook(int board[8][8], int side, int srcRank, int srcFile, int dstRank, int dstFile)
    {
		if (srcRank != dstRank && srcFile != dstFile) return false;//straight check
        int xDir = dstFile - srcFile;
        int yDir = dstRank - srcRank;
        bool vertical = false;
        if (xDir == 0) vertical = true;
        int xsign = 0;
        int ysign = 0;
        if (vertical) ysign = yDir / abs(yDir);
        else xsign = xDir / abs(xDir);
        int itVar = abs(yDir) + abs(xDir);
        for (int i = 1; i < itVar; i++)
        {
            int x = srcFile + i * xsign;
            int y = srcRank + i * ysign;
            if (board[y][x] != None) return false;
        }
        return true;
    }
    bool legalQueen(int board[8][8], int side, int srcRank, int srcFile, int dstRank, int dstFile)
    {
        //OR
        //if (legalRook(board, side, srcRank, srcFile, dstRank, dstFile)) return true;
        //else return legalBishop(board, side, srcRank, srcFile, dstRank, dstFile);
		return legalRook(board, side, srcRank, srcFile, dstRank, dstFile) || legalBishop(board, side, srcRank, srcFile, dstRank, dstFile) ;
	}
    void updateCastling(char casl = 'n')
    {
        if (kingMoved[0]) 
        {
            removeCastlingRight('Q');
            removeCastlingRight('K');
        }
        if (kingMoved[1])
        {
			removeCastlingRight('q');
			removeCastlingRight('k');
		}
        if (rookMoved[0]) removeCastlingRight('q');
        if (rookMoved[1]) removeCastlingRight('k');
        if (rookMoved[2]) removeCastlingRight('Q');
        if (rookMoved[3]) removeCastlingRight('K');
        if (casl == 'n') return;
		removeCastlingRight(casl);
    }
    bool legalKing(int board[8][8], int side, int srcRank, int srcFile, int dstRank, int dstFile, bool fr = true)
    {
        updateCastling();
        int xDir = dstFile - srcFile;
        int yDir = dstRank - srcRank;
        bool lazyKing = side == White ? srcRank == 7 && srcFile == 4 : srcRank == 0 && srcFile == 4;
        if (abs(yDir) > 1) return false;
        if (yDir == 0 && lazyKing && abs(xDir) == 2)
        {
            if (checkSq != -1) return false;
            int xsign = xDir / abs(xDir);
            int rookFile = xDir == 2 ? 7 : 0;
            for (int i = 0; i < dangerSquaresLen; i++)
            {
				if ((srcRank * 8)+srcFile + xsign == dangerSquares[i]) return false;
                if ((srcRank * 8) + srcFile + 2*xsign == dangerSquares[i]) return false;
                if (rookFile == 0 && (srcRank*8)+srcFile + 3 * xsign == dangerSquares[i]) return false;
			}
            int rookRank = side == White ? 7 : 0;
            char sideCastl = 'K';
            if (side == White) sideCastl = xsign == 1 ? 'K' : 'Q';
			else if (side == Black) sideCastl = xsign == 1 ? 'k' : 'q'; //k for upper left, q for upper right
            if (findChar(CASTLING, sideCastl) == -1) return false; //if you can castle return
            if (board[srcRank][rookFile] != Rook + side) return false; //can only castle with rook
            //fuck the for loop, doing this manually
            if(board[srcRank][srcFile + xsign] != None) return false; //sum in the way
            if (board[srcRank][srcFile + 2*xsign] != None) return false; // mmmm
            if (rookFile == 0 && board[srcRank][srcFile + 3 * xsign] != None) return false;
            if (fr)
            {
                board[rookRank][srcFile + (((7-rookFile) >> 2) + 3) * xsign] = None; //I'm kinda god like at coding IMO
                board[rookRank][srcFile + xsign] = Rook + side;
            }
            //(writes optimized code for a line called once every 100 runs) "na I'm actually different"
            return true;
        }
        if (abs(xDir) > 1) return false;
        return true;
	}
};