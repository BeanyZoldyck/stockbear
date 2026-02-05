#include "ofApp.h"
#include "ofMain.h"
#include <cstdlib>  
#include <math.h>
#include "HELPER.cpp"
int DEBUG = 0;
//just gonna use n instead of literal 8, for no reason at all
Board board;
//int boardOld[8][8];
//int boardOlder[8][8];
bool oneFold, twoFold;
int YDIS = 25;// WINHYT / 9;
int XDIS = 213;// WINLEN / 9;
bool AI = false;
ofColor BLACK(0, 0, 0);
ofColor WHITE(255, 255, 255);
ofColor Default[2] = { WHITE,BLACK };
ofColor LITEBROWN(196, 170, 137);
ofColor DARKBROWN(97, 68, 48);
ofColor WoodenTheme[2] = { LITEBROWN, DARKBROWN};
ofColor CHECKRED(174, 13, 45);
ofColor KINGCHECK(235, 30, 15);
int destIndex = -1;
ofRectangle TWOP = ofRectangle(WINLEN/3, WINHYT/3, WINLEN/3, 100);

ofRectangle BEARquotes = ofRectangle(WINLEN / 3, 2*WINHYT / 3, WINLEN / 3, 100);
ofRectangle BORDRECTS[64];

int boardWidth = WINLEN - 2*XDIS;
int squareLen = boardWidth / 8;

bool START = false;

ofImage whitePawn;
ofImage blackPawn;
ofImage whiteKnight;
ofImage blackKnight;
ofImage whiteRook;
ofImage blackRook;
ofImage whiteBishop;
ofImage blackBishop;
ofImage whiteKing;
ofImage blackKing;
ofImage whiteQueen;
ofImage blackQueen;
int selectIndex = -1;

ofImage blackPromoPieces[4];// 
ofImage whitePromoPieces[4];// = { whiteQueen, whiteKnight, whiteBishop, whiteRook };
string promo = "q";
bool gameContinue = true;

string GAMESTATE;


ofTrueTypeFont font;
/*16-(8-8) = 16
16-(16-8) = 8*/

void loadPNGS()
{
	font.loadFont("Analogist.ttf", 40);
	whitePawn.load("pieces/wpawn.png");
	blackPawn.load("pieces/bpawn.png");
	whiteKnight.load("pieces/wknight.png");
	blackKnight.load("pieces/bknight.png");
	whiteRook.load("pieces/wrook.png");
	blackRook.load("pieces/brook.png");
	whiteBishop.load("pieces/wbishop.png");
	blackBishop.load("pieces/bbishop.png");
	whiteKing.load("pieces/wking.png");
	blackKing.load("pieces/bking.png");
	whiteQueen.load("pieces/wqueen.png");
	blackQueen.load("pieces/bqueen.png");
	blackPromoPieces[0].load("pieces/bqueen.png");
	blackPromoPieces[1].load("pieces/bknight.png");
	blackPromoPieces[2].load("pieces/bbishop.png");
	blackPromoPieces[3].load("pieces/brook.png");
	whitePromoPieces[0].load("pieces/wqueen.png");
	whitePromoPieces[1].load("pieces/wknight.png");
	whitePromoPieces[2].load("pieces/wbishop.png");
	whitePromoPieces[3].load("pieces/wrook.png");
}
void drawPieces()
{
	ofSetColor(WHITE);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			int piece = board.board[i][j];
			if (piece == 0) continue;
			ofRectangle info = BORDRECTS[n*(i) +j];
			switch (piece)
			{
			case White+Pawn:
				whitePawn.draw(info.x,info.y,squareLen,squareLen);
				break;
			case White+Knight:
				whiteKnight.draw(info.x, info.y, squareLen, squareLen);
				break;
			case White+Bishop:
				whiteBishop.draw(info.x, info.y, squareLen, squareLen);
				break;
			case White+Rook:
				whiteRook.draw(info.x, info.y, squareLen, squareLen);
				break;
			case White+Queen:
				whiteQueen.draw(info.x, info.y, squareLen, squareLen);
				break;
			case White+King:
				whiteKing.draw(info.x, info.y, squareLen, squareLen);
				break;
			case Black+Pawn:
				blackPawn.draw(info.x, info.y, squareLen, squareLen);
				break;
			case Black+Knight:
				blackKnight.draw(info.x, info.y, squareLen, squareLen);
				break;
			case Black+Rook:
				blackRook.draw(info.x, info.y, squareLen, squareLen);
				break;
			case Black+Bishop:
				blackBishop.draw(info.x, info.y, squareLen, squareLen);
				break;
			case Black+King:
				blackKing.draw(info.x, info.y, squareLen, squareLen);
				break;
			case Black+Queen:
				blackQueen.draw(info.x, info.y, squareLen, squareLen);
				break;

			}
		}
	}
}


void makeBoardDraw()
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			BORDRECTS[n * (i)+j].set(j * squareLen + (XDIS), i * squareLen + (YDIS / 2), squareLen, squareLen);
			//BORDRECTS[n * (i)+j] = ofRectangle(j * squareLen + (XDIS), i * squareLen + (YDIS / 2), squareLen, squareLen);
		}
	}
}
void drawBoard()
{
	ofFill();
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			ofSetColor(WoodenTheme[((i + j) % 2)]);
			if (j + (n * i) == destIndex) ofSetColor(CHECKRED);
			else if (j + (n * i) == board.checkSq) ofSetColor(KINGCHECK);
			ofDrawRectangle(BORDRECTS[i * n + j]);
		}
	}
	if (board.dangerSquaresLen > 0 && DEBUG == 2)
	{
		ofSetColor(BLACK);
		for (int i = 0; i < board.dangerSquaresLen; i++)
		{
			ofDrawRectangle(BORDRECTS[board.dangerSquares[i]]);
		}
	}
	if (board.legalLen > 0 && DEBUG == 1)
	{
		ofSetColor(BLACK);
		for (int i = 0; i < board.legalLen; i++)
		{
			ofDrawRectangle(BORDRECTS[board.legalSquares[i]]);
		}
	}
}
void drawPromoBox()
{
	int boardEndPos = XDIS + 8 * squareLen;
	if (board.getPlayer() == Black) {
	for (int i = 0; i < 4; i++)
	{
		whitePromoPieces[i].draw(boardEndPos, YDIS/2 + squareLen*i, squareLen, squareLen);
	}
	}
	else
	{
		for (int i = 3; i >= 0; i--)
		{
			blackPromoPieces[3-i].draw(boardEndPos, (YDIS/2 + 7*squareLen) - (3-i)*squareLen, squareLen, squareLen);
		}
	
	}

}
int clampPiece(int yPos, int side)
{
	int pieceArray[4] = {Queen, Knight, Bishop, Rook};
	int piece = side == Black ? pieceArray[(yPos / squareLen)%4] : pieceArray[3-((yPos / squareLen) % 4)];
	return otherTeam(side) + piece;
}
void select(int x, int y)
{
	//TODO: Mathematical implementation that doesn't loop over board

	for (int i = 0; i < n * n; i++)
	{
		if (BORDRECTS[i].inside(x, y))
		{
			if (destIndex == i) destIndex = -1;//unaelect if they click selected square
			else
			{
				selectIndex = destIndex;
				destIndex = i;
			}
		}
	}
}
void print(int text)
{
	cout << text << endl;
}
void print(string text)
{
	cout << text << endl;
}
//bool sameAsTwoMovesAgo()
//{
//		for (int i = 0; i < n; i++)
//		{
//			for (int j = 0; j < n; j++)
//			{
//				if (board.board[i][j] != boardOlder[i][j]) return false;
//			}
//		}
//		return true;
//}
void setMatrixEqual(int inputMatrix[n][n], int copyFrom[n][n])
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			inputMatrix[i][j] = copyFrom[i][j];
		
		}
	}
}
bool enoughMats()//0 if suff. 
{
	int whitePieces = 0;
	int blackPieces = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int pieceS = board.board[i][j];
			if (pieceType(pieceS) == Pawn || pieceType(pieceS) == Queen) return true;
			if (pieceType(pieceS) == King) continue;
			if (piecesTeam(pieceS) == White) whitePieces += pieceType(pieceS);
			else blackPieces += pieceType(pieceS); //adds 0 if none
		}
	}
	if (whitePieces < Rook && blackPieces < Rook) return false;
	return true;
}
void move()
{
	if (!gameContinue) return;
	int moves = board.getMoves();
	if (AI && selectIndex != -1 && destIndex != -1 && piecesTeam(board.board[selectIndex / 8][selectIndex % 8]) != White) return;
	board.move(&selectIndex, &destIndex);
	bool moveMade = board.getMoves() != moves;
	//if (board.getPlayer() == Black && board.getMoves() != moves)
	//{
	//	setMatrixEqual(boardOlder, boardOld); // boardOlder = boardOld
	//	setMatrixEqual(boardOld, board.board); // boardOld = board
	//}
	//if (board.getPlayer() == Black && board.getMoves() != moves)
	//{
	//	board.printBoard(board.board);
	//	cout << "board" << endl;
	//	board.printBoard(boardOld);
	//	cout << "boardOld" << endl;
	//	board.printBoard(boardOlder);
	//	cout << "boardOlder" << endl;
	//	bool shitTheSame = sameAsTwoMovesAgo();
	//	if (shitTheSame && oneFold == false && twoFold == false)
	//	{
	//		oneFold = true;
	//	}
	//	else if (oneFold == true && twoFold == false && shitTheSame)
	//	{
	//		twoFold = true;
	//	}
	//	else if (oneFold == true && twoFold == true && shitTheSame)
	//	{
	//		board.setState(4);
	//		gameContinue = false;
	//		GAMESTATE = board.owariDa();
	//	}
	//	else
	//	{
	//		oneFold = false;
	//		twoFold = false;
	//	}
	//}
	board.checkSq = board.checkSquared(board.board);
	board.legalSquare(board.board);
	if (board.legalLen == 0)
	{
		if (board.checkSq == -1)
		{
			board.setState(1);		
		}
		else
		{
			board.setState(0);
		}
		gameContinue = false;
		GAMESTATE = board.owariDa();
	}
	else if (board.getHalfMoves() >= 50)
	{
		board.setState(3);
		gameContinue = false;
		GAMESTATE = board.owariDa();
	}
	else if (moveMade && !enoughMats())
	{
		board.setState(2);
		gameContinue = false;
		GAMESTATE = board.owariDa();
	}
	//print(board.legalLen);
}
void drawMenu()
{
	ofSetColor(38,8,138);
	ofDrawRectangle(0, 0, WINLEN, WINHYT);
	ofSetColor(WHITE);
	font.drawString("CHESS", WINLEN / 2 - 50, WINHYT / 4);
	ofDrawRectangle(TWOP);
	ofDrawRectangle(BEARquotes);
	ofSetColor(BLACK);
	font.drawString("2 player", TWOP.x+105, TWOP.y+55);
	font.drawString("AI Mode", BEARquotes.x + 105, BEARquotes.y + 55);

}
void startGame(int x, int y)
{
	if (TWOP.inside(x, y))
	{
		START = true;
	}
	else if (BEARquotes.inside(x, y))
	{
		AI = true;
		START = true;
	}
}
//--------------------------------------------------------------
void ofApp::setup()
{
	//cout << 8*squareLen << endl;
	ofSetFrameRate(20);
	ofSetBackgroundColor(48,46,43);
	board.setBoard();
	makeBoardDraw();
	loadPNGS();
	//tring BORDFEN = boardToFEN(board, 16, CASTLING, "", 0, 1);
	//drawPieces();
	//printBoard(n, board);
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (board.getPlayer() != White && AI && gameContinue)
	{
		board.moveAIa(board.getPlayer());
		selectIndex = -1;
		destIndex = -1;
	}
	if (board.promoFlag > Black)
	{ 
		int playerBeforePromo = board.getPlayer();
		board.promote();
		move();//update temp board
		board.swapSides();
		board.possSquares(board.board); //whites turf
		board.swapSides();
		board.checkSq = board.checkSquared(board.board);
		if (board.checkSq != -1) board.legalSquare(board.board);
		if (board.checkSq == -1) board.swapSides(); //TODO: automatic check detection after promotion
		board.promoFlag = -1;
		if (board.getPlayer() != playerBeforePromo) board.swapSides();
	}

}

//--------------------------------------------------------------
void ofApp::draw()//every frame draw is called btw, duhh
{
	//ofNoFill();
	drawBoard();
	drawPieces();
	if (board.promoFlag != -1)
	{
		drawPromoBox();
	}
	if (!gameContinue)
	{
		board.getPlayer() == White ? ofSetColor(BLACK) : ofSetColor(WHITE);

		font.drawString(GAMESTATE, XDIS+squareLen, YDIS+4*squareLen);
	}
	if (!START)
	{
		drawMenu();
	}
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == OF_KEY_F4)
	{
		board.setBoard();
		gameContinue = true;
		oneFold = false;
		twoFold = false;
		START = false;
		AI = false;

	}
	else if (key == OF_KEY_F5)
	{
		DEBUG =	 1;	
	}
	else if (key == OF_KEY_F6)
	{
		DEBUG = 2;
	}
	else if (key == OF_KEY_F7)
	{
		DEBUG = 0;
	}
	else if (key == ' ')
	{
		destIndex = -1;
		selectIndex = -1;
		if (!gameContinue) {
			board.setBoard(); gameContinue = 1; }
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!START)
	{
		startGame(x,y);
		return;
	}
	if (board.promoFlag == -1)
	{
		select(x, y);
		move();
	}
	else
	{
		y -= YDIS / 2;
		if (board.getPlayer() == White && y < squareLen * 4) return;
		if (board.getPlayer() == Black && y > squareLen * 4) return;
		if (x < XDIS + 8 * squareLen) return;
		board.promoFlag = clampPiece(y, board.getPlayer());
		return;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    //ofSetFrameRate(fps);
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    //ofSetFrameRate(fps/2);

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	//boardWidth = w - 2 * XDIS;
	//squareLen = boardWidth / 8;
	//makeBoardDraw();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}