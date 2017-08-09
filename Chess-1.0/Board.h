#ifndef BOARD_H
#define BOARD_H
#include <Stone.h>

#include <QWidget>

class Board : public QWidget
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = 0);

    bool _bRedturn;
    int _r;//棋子半径
    int _selectid;
    QPoint center(int row, int col);//返回象棋中心坐标
    QPoint center(int id);

    void paintEvent(QPaintEvent *);//画棋盘
    Stone _s[32];
    void drawStone(QPainter &painter , int id);
    void mouseReleaseEvent(QMouseEvent *ev);
    bool getRowcol (QPoint pt, int &row, int &col);
    bool canMove1 (int moveid, int row, int col, int killid);
    bool canMove2 (int moveid, int row, int col, int killid);
    bool canMove3 (int moveid, int row, int col, int killid);
    bool canMove4 (int moveid, int row, int col, int killid);
    bool canMove5 (int moveid, int row, int col, int killid);
    bool canMove6 (int moveid, int row, int col, int killid);
    bool canMove7(int moveid, int row, int col, int killid);

    bool canMove(int moveid, int row, int col, int killid);
    int getStoneId(int row, int col);
    int getStoneCountAtLine(int row1, int col1, int row2, int col2);
    bool isDead(int id);

signals:

public slots:
     void initi();

};

#endif // BOARD_H
