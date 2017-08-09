#include "Board.h"
#include "Stone.h"
#include <QPainter>
#include <QMouseEvent>
#include <QSound>
#include <QTimer>
#define GetRowCol(__row, __col, __id) \
    int __row = _s[__id]._row;\
    int __col = _s[__id]._col

Board::Board(QWidget *parent) :
    QWidget(parent)
{
    QSound *sound = new QSound("chess.wav", this); //构建对象
    sound->play();//播放
    sound->setLoops(-1);//设置循环次数
 // sound->stop();//停止

    this->setWindowTitle(tr("金亚平制作"));
    for(int i=0; i<32; ++i)
       _s[i].init(i);
    _selectid = -1;
    _bRedturn = true;
}

QPoint Board::center(int row, int col)
{
    QPoint ret;
    ret.rx() = (col+1)*_r*2;
    ret.ry() = (row+1)*_r*2;
    return ret;
}

QPoint Board::center(int id)
{
    return center(_s[id]._row, _s[id]._col);
}

void Board::initi()
{
    for(int i=0; i<32; ++i)
       _s[i].init(i);
    _selectid = -1;
    _bRedturn = true;
    update();
}

void Board::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int d = 40;	//确定棋子直径
    _r = d/2;
    // 画10横线
    for(int i=1; i<=10; ++i)
    {
        painter.drawLine(QPoint(d, i*d), QPoint(9*d, i*d));
    }
    // 画9竖线
    for(int i=1; i<=9; ++i)
    {
        if(i==1 || i==9)
           painter.drawLine(QPoint(i*d, d), QPoint(i*d, 10*d));
        else
        {
           painter.drawLine(QPoint(i*d, d), QPoint(i*d, 5*d));
           painter.drawLine(QPoint(i*d, 6*d), QPoint(i*d, 10*d));
        }
    }

    // 绘制九宫格
    painter.drawLine(QPoint(4*d, 1*d), QPoint(6*d, 3*d));
    painter.drawLine(QPoint(6*d, 1*d), QPoint(4*d, 3*d));

    painter.drawLine(QPoint(4*d, 8*d), QPoint(6*d, 10*d));
    painter.drawLine(QPoint(6*d, 8*d), QPoint(4*d, 10*d));
	
	//绘制“楚河汉界”
	QPointF point(_r*5.5, _r*11.5);
	painter.setFont(QFont("Arial", 20));
	painter.drawText(point, "楚河            汉界");

    //绘制32个棋子
    for(int i=0 ; i <32 ; ++i)
    {
        drawStone(painter, i);
    }
	
	//黑棋或红旗的“将”死亡时，出现“*棋胜利”并等待三秒重绘棋盘
    if(_s[4]._dead || _s[20]._dead)
    {
        if(_s[4]._dead)
        {
            QPointF point(_r*20, _r*11.5);
            painter.setFont(QFont("Arial", 50));

            painter.drawText(point, "黑棋胜");
        }
        if(_s[20]._dead)
        {
            QPointF point(_r*20, _r*11.5);
            painter.setFont(QFont("Arial", 50));

            painter.drawText(point, "红棋胜");
        }
        QTimer::singleShot(3000, this, SLOT(initi()));	//定时器函数，等待三秒运行槽函数 initi()重绘棋盘
    }





}

void Board::drawStone(QPainter &painter, int id)
{
    if(_s[id]._dead)
        return;			//若棋子死亡，不显示
    QPoint c = center(id);	//获取棋子坐标
    QRect rect = QRect(c.x()-_r, c.y()-_r, _r*2, _r*2);	//为棋子中显示字体（如：将）设置矩形区域

    if(id == _selectid)
    {
        painter.setBrush(QBrush(QColor(188, 143, 143))); //被选中的棋子的颜色
        painter.setPen(Qt::green);	//棋子背景色
    }
    else
         painter.setBrush(QBrush(QColor(127, 255, 212)));//未被选中的棋子的颜色

    painter.setPen(Qt::black);		

    painter.drawEllipse(center(id), _r, _r);//画圆
    if(_s[id]._red)		//若为红旗则为红色
         painter.setPen(Qt::red);
    painter.setFont(QFont("system", _r, 700));	//棋子字体大小
    painter.drawText(rect, _s[id].getText(), QTextOption(Qt::AlignCenter));//写汉字

}

void Board::mouseReleaseEvent(QMouseEvent *ev)
{
    QPoint pt = ev->pos();
    int row, col;
    bool bRet = getRowcol(pt, row, col);
    if(bRet == false)   //点到棋盘外
        return;

    int i;
    int clickid = -1;

    for(i = 0; i<32; i++)
    {
        if(_s[i]._row == row && _s[i]._col == col && _s[i]._dead == false)
            break;
    }

    if(i < 32)
    {
        clickid = i;
        update();
    }
    //选棋
    if(_selectid == -1)
    {
        if(clickid != -1)
        {
            if(_bRedturn == _s[clickid]._red)
            {
                _selectid = clickid;
                update();
            }
        }
    }
    //走棋
    else
    {
        if(canMove(_selectid, row, col, clickid))
        {
            _s[_selectid]._col = col;
            _s[_selectid]._row = row;
            if(clickid != -1)
            {
                _s[clickid]._dead = true;
            }
            _selectid = -1;
            _bRedturn = ! _bRedturn;    //轮换
            update();
        }

    }

}

bool Board::getRowcol(QPoint pt, int &row, int &col)
{
    for(row = 0; row<=9; row++)
    {
        for(col = 0; col<=8; col++)
        {
            QPoint c = center(row, col);
            int dx = c.x() - pt.x();
            int dy = c.y() - pt.y();
            int dist = dx*dx+dy*dy;
            if(dist < _r*_r)
                return true;
        }
    }
    return false;

}
//车
bool Board::canMove1(int moveid, int row, int col, int killid)		//“车”的走法
{
    GetRowCol(row1, col1, moveid);	//获取棋子行列级及其id
    int ret = getStoneCountAtLine(row1, col1, row, col);	//查看直线上是否有棋子
    if(ret == 0)
        return true;
    return false;
}
//马
bool Board::canMove2(int moveid, int row, int col, int killid)
{
//计算被选择棋子与要走的坐标之间的距离进而对其进行“日”字限制
    GetRowCol(row1, col1, moveid);
    int dr = _s[moveid]._row - row;
    int dc = _s[moveid]._col - col;
    int d = abs(dr)*10 + abs(dc);
//蹩马腿限制
    if(d == 12 || d == 21)
    {
        if(d == 12)
        {
            if(getStoneId(row1, (col+col1)/2) != -1)
                return false;
        }
        else
        {
            if(getStoneId((row+row1)/2, col1) != -1)
                return false;
        }

         return true;
    }
    else
    {
         return false;
    }

    return true;
}

//象
bool Board::canMove3(int moveid, int row, int col, int killid)
{
    //“象”不能过河，对其进行行数限制
    if(_s[moveid]._red)		//红“象”
    {
        if(row > 4)
            return false;
    }
    else					//黑“象”
    {
        if(row < 5)
            return false;
    }
    //象眼限制
    int rEye = (row+_s[moveid]._row)/2;
    int cEye = (col+_s[moveid]._col)/2;
    if(getStoneId(rEye, cEye) != -1)
        return false;

    //“田”字限制
    int dr = _s[moveid]._row - row;
    int dc = _s[moveid]._col - col;
    int d = abs(dr)*10 + abs(dc);
    if(d == 22)
    {
         return true;
    }
    else
    {
         return false;
    }

}

//士
bool Board::canMove4(int moveid, int row, int col, int killid)
{
	//对红“士”进行“田”限制
    if(_s[moveid]._red)		
    {
        if(row > 2)
            return false;
    }
    else
    {
        if(row < 7)
            return false;
    }
    if(col < 3 || col > 5)
        return false;
	
	//走斜线限制
    int dr = _s[moveid]._row - row;
    int dc = _s[moveid]._col - col;
    int d = abs(dr)*10 + abs(dc);
    if(d == 11)
    {
         return true;
    }
    else
    {
         return false;
    }

}

//将
bool Board::canMove5(int moveid, int row, int col, int killid)
{
	//判断是否出现“龙见龙”的情况
    if(killid != -1 && _s[killid]._type == Stone::JIANG)
    {
        GetRowCol(row1, col1, moveid);
        int ret = getStoneCountAtLine(row1, col1, row, col);	
        if(ret == 0)
            return true;		//若是，则直接吃掉
        return false;
    }

    if(_s[moveid]._red)				//红“将”“田”字限制
    {
        if(row > 2)
            return false;
    }
    else							//黑“将”“田”字限制
    {
        if(row < 7)
            return false;
    }
    if(col < 3 || col > 5)
        return false;
	
	//一次只能走一步
    int dr = _s[moveid]._row - row;
    int dc = _s[moveid]._col - col;
    int d = abs(dr)*10 + abs(dc);

    if(d != 1 && d != 10) return false;
}

//炮
bool Board::canMove6(int moveid, int row, int col, int killid)
{
    GetRowCol(row1, col1, moveid);		
    int ret = getStoneCountAtLine(row, col, row1, col1);
	
	//对炮走的路径上是否 隔子 进行判断并设置
    if(killid != -1)
    {
        if(ret == 1) return true;
    }
    else
    {
        if(ret == 0) return true;
    }
    return false;
}

//兵
bool Board::canMove7(int moveid, int row, int col, int killid)
{
    GetRowCol(row1, col1, moveid);
	//步数限制
    int dr = _s[moveid]._row - row;
    int dc = _s[moveid]._col - col;
    int d = abs(dr)*10 + abs(dc);
    if(d != 1 && d != 10) return false;
	//对兵进行列和过河前后走法限制
    if(_s[moveid]._red)
    {
        if(row < row1) return false;    //不能后退
        if(row1 <= 4 && row == row1) return false;//过河前不能左右
    }
    else
    {
        if(row > row1) return false;
        if(row1 >= 5 && row == row1) return false;
    }
    return true;
}

bool Board::canMove(int moveid, int row, int col, int killid)
{
    if(_s[moveid]._red == _s[killid]._red)	//确定是选择棋子还是走棋
    {
        _selectid = killid;
        update();
        return false;
    }
    switch(_s[moveid]._type)		//确定棋子类型进而确定其走法
    {
        case Stone::CHE :
            return  canMove1(moveid, row, col, killid);
            break;
        case Stone::MA :
            return  canMove2(moveid, row, col, killid);
            break;
        case Stone::XIANG :
            return   canMove3(moveid, row, col, killid);
            break;
        case Stone::SHI :
            return   canMove4(moveid, row, col, killid);
            break;
        case Stone::JIANG :
            return  canMove5(moveid, row, col, killid);
            break;
        case Stone::PAO :
            return   canMove6(moveid, row, col, killid);
            break;
        case Stone::BING :
            return  canMove7(moveid, row, col, killid);
            break;
    }
    return true;
}

//获取棋子id
int Board::getStoneId(int row, int col)
{
    for(int i=0; i<32; ++i)
    {
        if(_s[i]._row == row && _s[i]._col == col && !isDead(i))
            return i;
    }
    return -1;
}

//获取棋子行列值
int Board::getStoneCountAtLine(int row1, int col1, int row2, int col2)
{
    int ret = 0;
    if(row1 != row2 && col1 != col2)
        return -1;
    if(row1 == row2 && col1 == col2)
        return -1;

    if(row1 == row2)
    {
        int min = col1 < col2 ? col1 : col2;
        int max = col1 < col2 ? col2 : col1;
        for(int col = min+1; col<max; ++col)
        {
            if(getStoneId(row1, col) != -1) ++ret;
        }
    }
    else
    {
        int min = row1 < row2 ? row1 : row2;
        int max = row1 < row2 ? row2 : row1;
        for(int row = min+1; row<max; ++row)
        {
            if(getStoneId(row, col1) != -1) ++ret;
        }
    }

    return ret;
}

//判断棋子是否死亡
bool Board::isDead(int id)
{
    if(id == -1)return true;
    return _s[id]._dead;
}
