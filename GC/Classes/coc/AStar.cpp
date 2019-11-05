#include "AStar.h"
#include "SceneBgr.h"
#include "PlayerScene.h"
#include <algorithm>

NS_COC_BEGIN

#define NODE_COST 10
#define NODE_COST2 14
#define STATE_OPEN  1
#define STATE_CLOSE 2

AStar::AStar()
{
    init();
}

AStar::~AStar()
{
    
}

void AStar::init()
{
    for (int x = 0; x < LOGIC_W; ++x) {
        for(int y = 0; y < LOGIC_W; ++y)
        {
            m_mapNode[x][y].x = x;
            m_mapNode[x][y].y = y;
        }
    }
}

bool AStar::hasNode(AStarNodeList& lst, const AStarNode& node)
{
    for(AStarNodeList::const_iterator it = lst.begin(); it !=lst.end(); ++it)
    {
        if(it->x == node.x && it->y == node.y)
            return true;
    }
    
    return false;
}

const AStarNode AStar::findNode(AStarNodeList& lst, int x, int y)
{
    AStarNode node;
    
    for(AStarNodeList::const_iterator it = lst.begin(); it !=lst.end(); ++it)
    {
        if(it->x == node.x && it->y == node.y)
            return *it;
    }
    
    return node;
}

int AStar::countDistance(int bx, int by, int ex, int ey)
{
    return (bx - ex) * (bx - ex) + (by - ey) * (by - ey);
}

bool AStar::procDir(int& dir, int& px, int& py, int& cost, int x, int y)
{
    if(dir > 7)
        return false;
    
    switch(dir)
    {
        case 0: //RIGHT
        {
            px = x - 1;
            py = y;
            
            cost = NODE_COST;
        }
            break;
        case 1: //RIGHT-UP
        {
            px = x - 1;
            py = y - 1;
            
            cost = NODE_COST2;
        }
            break;
        case 2: //UP
        {
            px = x;
            py = y - 1;
            
            cost = NODE_COST;
        }
            break;
        case 3: //UP-LEFT
        {
            px = x + 1;
            py = y - 1;
            
            cost = NODE_COST2;
        }
            break;
        case 4: //LEFT
        {
            px = x + 1;
            py = y;
            
            cost = NODE_COST;
        }
            break;
        case 5: //LEFT-DOWN
        {
            px = x + 1;
            py = y + 1;
            
            cost = NODE_COST2;
        }
            break;
        case 6: //DOWN
        {
            px = x;
            py = y + 1;
            
            cost = NODE_COST;
        }
            break;
        case 7:  //DOWN-RIGHT
        {
            px = x - 1;
            py = y + 1;
            
            cost = NODE_COST2;
        }
            break;
    }
    
    ++dir;
    
    return true;
}

bool AStar::isWall(int x, int y)
{
    int bid = m_map.get(x, y) / 100;
    
    return bid == 103;
}

void AStar::pushOpenSet(AStarNode *preNode, AStarNode *curNode)
{
    int px = curNode->x;
    int py = curNode->y;
    
    m_mapNode[px][py].step = preNode->step + 1;
    m_mapNode[px][py].preNode = preNode;
    
    m_setOpen.insert(m_mapNode[px][py]);
    m_mapState.set(px, py, STATE_OPEN);
}

void AStar::pushCloseSet(AStarNode *preNode, AStarNode *curNode)
{
    int px = curNode->x;
    int py = curNode->y;
    
    m_mapNode[px][py].step = preNode->step + 1;
    m_mapNode[px][py].preNode = preNode;
    
    m_setClose.insert(m_mapNode[px][py]);
    m_mapState.set(px, py, STATE_CLOSE);
}

bool AStar::search(int bx, int by, int& ex, int& ey, float length, bool bCanJumpWall, bool isDestWall, int maxfar, int mywidth)
{
    m_resultOK = false;
    
    if(bx < 0)
        bx = 0;
    if(bx >= m_map.m_width)
        bx = m_map.m_width - 1;
    
    if(by < 0)
        by = 0;
    if(by >= m_map.m_height)
        by = m_map.m_height - 1;
    
    if(ex < 0)
        ex = 0;
    if(ex >= m_map.m_width)
        ex = m_map.m_width - 1;
    
    if(ey < 0)
        ey = 0;
    if(ey >= m_map.m_height)
        ey = m_map.m_height - 1;
    
    m_lstResult.clear();
    
    bool needJump = bCanJumpWall;
    
    if(bx == ex && by == ey)
    {
        return true;
    }
    
    m_setOpen.clear();
    m_setClose.clear();
    m_mapState.cleanup();
    
    int ox = abs(ex - bx);
    int oy = abs(ey - by);
    
    int line_step = sqrt((float)ox * ox + oy * oy);
    
    AStarNode* start = m_mapNode[bx][by].reset();
    AStarNode* end = NULL;
    
    m_setOpen.insert(*start);
    m_mapState.set(bx, by, STATE_OPEN);
    
    int px, py;
    bool isfind = false;
    
    int len2 = length * length;
    int dir = 0, cost = 0;
    
    while(!m_setOpen.empty())
    {
        _Set::iterator it = m_setOpen.begin();
        
        AStarNode* curnode = &m_mapNode[it->x][it->y];
        m_setClose.insert(*curnode);
        m_mapState.set(curnode->x, curnode->y, STATE_CLOSE);
        m_setOpen.erase(m_setOpen.begin());
        
        if(length == 0)
        {
            if(curnode->x == ex && curnode->y == ey)
                isfind = true;
        }
        else
        {
            int ox = abs(curnode->x - ex);
            int oy = abs(curnode->y - ey);
            int len = ox * ox + oy * oy;
            if(len <= len2)
            {
                isfind = true;
            }
        }
        
        if(isfind || curnode->step > line_step + maxfar)
        {
            end = curnode;
            break;
        }
        
        dir = 0;
        while(procDir(dir, px, py, cost, curnode->x, curnode->y))
        {
            if(canMove(curnode->x, curnode->y, px, py, mywidth, needJump))
            {
                int state = m_mapState.get(px, py);
                
                if(state == 0)
                {
                    if(isWall(px, py)) {
                        cost += NODE_COST + LOGIC_METER;
                    }
                    
                    int costG = curnode->costG + cost;
                    int costH = (abs(px - ex) + abs(py - ey)) * LOGIC_METER;
                    
                    m_mapNode[px][py].costG = costG;
                    m_mapNode[px][py].costH = costH;
                    m_mapNode[px][py].costF = costG + costH;
                    
                    pushOpenSet(curnode, &m_mapNode[px][py]);
                }
            }
        }
    }
    
    if (isfind) {
        
        ex = end->x;
        ey = end->y;
        
        m_resultOK = procCloseList(start, end, length, bCanJumpWall, isDestWall, maxfar, mywidth);
    }
    
    return m_resultOK;
}

list<AStarNode> _pathNode;
bool AStar::tryMove(int bx, int by, int ex, int ey, int mywidth, bool canJump)
{
    _pathNode.clear();
    bool canmove = true;
    
    while (bx != ex || by != ey) {
        
        if (bx < ex) {
            bx ++;
        }
        else if (bx > ex) {
            bx --;
        }
        
        if (by < ey) {
            by ++;
        }
        else if (by > ey) {
            by --;
        }
        
        if (!canMove(bx, by, bx, by, mywidth, false) || isWall(bx, by)) {
            canmove = false;
            break;
        }
        
        AStarNode node;
        node.x = bx;
        node.y = by;
        
        _pathNode.push_back(node);
    }
    
    return canmove;
}

bool AStar::canMove(int bx, int by, int ex, int ey, int mywidth, bool canJump)
{
    bool isvalid = false;
    
    if(mywidth > 1)
    {
        if(m_map.isValidPos(ex, ey))
        {
            int ox = ex - bx;
            int oy = ey - by;
            int dx = ex + ox;
            int dy = ey + oy;
            
            isvalid = m_map.isValidPos(dx, dy);
        }
    }
    else
        isvalid = m_map.isValidPos(ex, ey);
    
    bool canmove = false;
    
    if (isvalid) {
        
        if(m_map.get(ex, ey) == 0)
            canmove = true;
        else if(canJump)
        {
            int bid = m_map.get(ex, ey) / 100;
            if(bid == BUILDING_WALL)
                canmove = true;
        }
    }
    
    return canmove;
}

// 如果终点坐标不可达，则跳转到最接近起点的一个可达坐标
void AStar::procDestPt(int bx, int by, int& ex, int& ey)
{
    if(bx == ex && by == ey)
        return;
    
    float ox = 1.0f;
    float oy = 1.0f;
    
    if(abs(ex - bx) > abs(ey - by))
    {
        oy = (ey - by) / abs(ex - bx);
        ox = (ex - bx > 0) ? 1.0f : -1.0f;
        
        float ty = ey;
        for(int i = 0; i < abs(ex - bx); ++i)
        {
            if(m_map.get(ex, ty) == 0)
            {
                ey = ty;
                
                return ;
            }
            
            ex -= ox;
            ty -= oy;
        }
    }
    else
    {
        ox = (ex - bx) / abs(ey - by);
        oy = (ey - by > 0) ? 1.0f : -1.0f;
        
        float tx = ex;
        for(int i = 0; i < abs(ey - by); ++i)
        {
            if(m_map.get(tx, ey) == 0)
            {
                ex = tx;
                
                return ;
            }
            
            ey -= oy;
            tx -= ox;
        }
    }
}

// 最后倒着走一遍close节点
bool AStar::procCloseList(AStarNode* startNode, AStarNode* endNode, int length, bool bCanJumpWall, bool isDestWall, int maxfar, int mywidth)
{
    m_lstResult.clear();
    
    if(m_setClose.empty())
        return false;
    
    int nodedis1 = 0;
    int nodedis2 = 0;
    
    while (!(endNode == startNode)) {
        
        nodedis1 = abs(endNode->x - startNode->x) + abs(endNode->y - startNode->y);
        
        if (endNode->preNode) {
            
            AStarNode* preNode = endNode->preNode;
            
            nodedis2 = abs(preNode->x - startNode->x) + abs(preNode->y - startNode->y);
            
            if (nodedis2 > nodedis1) {
                
                if (tryMove(endNode->x, endNode->y, startNode->x, startNode->y, mywidth, bCanJumpWall))
                {
                    list<AStarNode>::iterator it = _pathNode.begin();
                    for (; it != _pathNode.end(); ++ it) {
                        m_lstResult.push_front(*it);
                    }
                    
                    break;
                }
            }
        }
        
        m_lstResult.push_front(*endNode);
        endNode = endNode->preNode;
    }
    
    return true;
}

bool AStar::procResult(int kx, int ky)
{
    for(AStarNodeList::iterator it = m_lstResult.begin(); it != m_lstResult.end(); )
    {
        if(it->x == kx && it->y == ky)
            break;
        
        it = m_lstResult.erase(it);
    }
    
    for(AStarNodeList::reverse_iterator it = m_lstResult2.rbegin(); it != m_lstResult2.rend(); ++it)
    {
        AStarNode cn = *it;
        m_lstResult.push_front(cn);
    }
    
    return true;
}

bool AStar::findKeyPt_Result(int& ptx, int& pty)
{
    bool bFind = false;
    int lastcost = 999999999;
    for(AStarNodeList::iterator it = m_lstResult.begin(); it != m_lstResult.end(); ++it)
    {
        if(!bFind)
        {
            if(it->costF > lastcost)
                bFind = true;
            
            lastcost = it->costF;
            ptx = it->x;
            pty = it->y;
        }
        else
        {
            if(it->costF < lastcost)
                return true;
            
            lastcost = it->costF;
            ptx = it->x;
            pty = it->y;
        }
    }
    
    return false;
}

void AStar::rebuildMapArea()
{
    m_mapArea.cleanup();
    
    int curareaindex = 1;
    for(int y = 0; y < m_mapArea.m_height; ++y)
    {
        for(int x = 0; x < m_mapArea.m_width; ++x)
        {
            int ai = _getCurAreaIndex(x, y);
            if(ai == -1)
                m_mapArea.set(x, y, -1);
            else if(ai == 0)
                m_mapArea.set(x, y, curareaindex++);
            else
                m_mapArea.set(x, y, ai);
            
            int id0;
            int id1;
            if(_isSameArea(x, y, id0, id1))
            {
                _chgArea(x, y, MAX(id0, id1), MIN(id0, id1));
            }
        }
    }
    
    //    saveMapArea();
}

// 判断2个区域是联通的
bool AStar::_isSameArea(int x, int y, int& id0, int& id1)
{
    if(m_mapArea.get(x, y) > 0)
    {
        if(y > 0 && x > 0)
        {
            id0 = m_mapArea.get(x, y - 1);
            id1 = m_mapArea.get(x - 1, y);
            
            if(id0 > 0 && id1 > 0)
                return id0 != id1;
            
            id0 = m_mapArea.get(x - 1, y - 1);
            id1 = m_mapArea.get(x, y);
            
            if(id0 > 0 && id1 > 0)
                return id0 != id1;
            
            id0 = m_mapArea.get(x + 1, y - 1);
            id1 = m_mapArea.get(x, y);
            
            if(id0 > 0 && id1 > 0)
                return id0 != id1;
        }
    }
    
    return false;
}

// 改变区域id
void AStar::_chgArea(int mx, int my, int id0, int id1)
{
    for(int y = 0; y < m_mapArea.m_height; ++y)
    {
        for(int x = 0; x < m_mapArea.m_width; ++x)
        {
            if(m_mapArea.get(x, y) == id0)
                m_mapArea.set(x, y, id1);
            
            if(x >= mx && y >= my)
                return;
        }
    }
}

// 这个函数依赖于rebuildMapArea算法，不能随便用的
int AStar::_getCurAreaIndex(int x, int y)
{
    if(m_map.get(x, y) > 0)
        return -1;
    
    int iy = 0;
    int ix = 0;
    
    if(y > 0)
    {
        iy = m_mapArea.get(x, y - 1);
        if(iy > 0)
            return iy;
    }
    
    if(x > 0)
    {
        ix = m_mapArea.get(x - 1, y);
        if(ix > 0)
            return ix;
    }
    
    return 0;
}

void AStar::saveMap()
{
    FILE* fp = fopen("map.txt", "w");
    if(fp != NULL)
    {
        for(int y = 0; y < m_map.m_height; ++y)
        {
            for(int x = 0; x < m_map.m_width; ++x)
            {
                fprintf(fp, "%s", m_map.get(x, y) > 0 ? "1" : "0");
            }
            
            fprintf(fp, "\r\n");
        }
        
        fclose(fp);
    }
}

void AStar::saveMapArea()
{
    char view[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    
    std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();
    
    std::string file = path + "maparea.txt";
    
    FILE* fp = fopen(file.c_str(), "w");
    if(fp != NULL)
    {
        for(int y = 0; y < m_mapArea.m_height; ++y)
        {
            for(int x = m_mapArea.m_width - 1; x >= 0; --x)
            {
                if(m_mapArea.get(x, y) < 0)
                    fprintf(fp, "-");
                else if(m_mapArea.get(x, y) == 0)
                    fprintf(fp, "=");
                else
                    fprintf(fp, "%c", view[m_mapArea.get(x, y)]);
            }
            
            fprintf(fp, "\r\n");
        }
        
        fclose(fp);
    }
}

NS_COC_END
