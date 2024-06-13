#include <ios>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>


#define RADIUS 40
#define DEBUGMODE false

using namespace std;

sf::RenderWindow window(sf::VideoMode(1200, 900), "Dijkstra's Algorithm Visualization", sf::Style::Close | sf::Style::Resize);
sf::Font roboto_font;


struct Node{
    sf::Vector2f position;
    string name;
    unordered_map<string,int> connectedTo;
};

struct Graph{
    vector<Node> nodes;
    int startNodeIndex = 0;
};
void connect_nodes(Node& a1, Node& a2, int edgeLen){
    a1.connectedTo[a2.name] = edgeLen;
    //a2.connectedTo[a1.name] = edgeLen; // UNCOMMENT THIS TO MAKE IT UNDIRECTED
}

struct ui_sketches{
    vector<sf::VertexArray> lines;
};

void print_graph(const Graph& graph){
    cout << endl;
    for (const auto& x : graph.nodes){
        for (const auto& y : x.connectedTo) {
            cout << x.name << " --"<< y.second <<"-->  " << y.first << " " << endl;
        }
        cout << endl;
    }
    cout << endl;
}

unordered_map<string,int> Dijkstra(Graph G, const Node& start){
    bool visited[G.nodes.size()];
    bool stillHaveUnvisited = true;
    unordered_map<string, int> newDistances;
    for (int i = 0; i < G.nodes.size(); ++i) {
        visited[i] = false;
        newDistances[G.nodes[i].name] = INT_MAX - 1;
        if(G.nodes[i].name == start.name){
            newDistances[start.name] = 0;
        }
    }
    while(stillHaveUnvisited){
        int min = INT_MAX;
        int i = 0;
        int curindex = 0;
        for (; i < G.nodes.size(); i++) {   // LİSTEDEKİ ZİYARET EDİLMEMİŞ EN KÜÇÜK DEĞERİ SEÇMEK İÇİN
            if (!visited[i] && newDistances[G.nodes[i].name] < min){
                min = newDistances[G.nodes[i].name];
                curindex = i;
            }
        }
        visited[curindex] = true;
        for (auto connectedOnes : G.nodes[curindex].connectedTo) { // SEÇİLENİN KOMŞULARINA YENİ DEĞER VERMEK İÇİN
            if(connectedOnes.second < newDistances[connectedOnes.first] )
                newDistances[connectedOnes.first] = connectedOnes.second + newDistances[G.nodes[curindex].name];
        }
        stillHaveUnvisited = false;
        for (bool hm : visited) {
            if (!hm){
                stillHaveUnvisited = true;
            }
        }
    }
    return newDistances;
}


void draw_board(Graph G, const ui_sketches& ui_sketch);
bool isMouseOnAnyNode(Graph G);

void ui_create_node(Graph& graph){
    Node new_real_node;
    new_real_node.name = char(graph.nodes.size() + 'A');
    new_real_node.position = (sf::Vector2f)sf::Mouse::getPosition(window);
    graph.nodes.push_back(new_real_node);
}
void ui_create_edge( Graph& G, const ui_sketches& ui_sketches){
    sf::Vector2f old_mouse_pos = (sf::Vector2f)sf::Mouse::getPosition(window);

    int fromIndex = 999;
    for (int i = 0; i < G.nodes.size(); ++i) {
        if((G.nodes[i].position.x - old_mouse_pos.x)*(G.nodes[i].position.x - old_mouse_pos.x) + (G.nodes[i].position.y - old_mouse_pos.y)*(G.nodes[i].position.y - old_mouse_pos.y) <= RADIUS*RADIUS){
            fromIndex = i;
        }
    }
    if(fromIndex == 999)
        cout << "something went bad! When creating edge, it couldn't find the from index";


    while(true){ //Animation, selection of the second node, finalization
        sf::Vector2f cur_mouse_pos = (sf::Vector2f)sf::Mouse::getPosition(window);
        sf::VertexArray lines(sf::LinesStrip, 2);
        lines[0].position = sf::Vector2f(G.nodes[fromIndex].position.x, G.nodes[fromIndex].position.y);
        lines[1].position = sf::Vector2f(cur_mouse_pos.x,cur_mouse_pos.y);
        lines[1].color = sf::Color::Red;



        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && ((G.nodes[fromIndex].position.x - cur_mouse_pos.x)*(G.nodes[fromIndex].position.x - cur_mouse_pos.x) + (G.nodes[fromIndex].position.y - cur_mouse_pos.y)*(G.nodes[fromIndex].position.y - cur_mouse_pos.y) > RADIUS*RADIUS)){
            if(isMouseOnAnyNode(G)){
                int toIndex = 999;

                for (int i = 0; i < G.nodes.size(); ++i) {
                    if((G.nodes[i].position.x - cur_mouse_pos.x)*(G.nodes[i].position.x - cur_mouse_pos.x) + (G.nodes[i].position.y - cur_mouse_pos.y)*(G.nodes[i].position.y - cur_mouse_pos.y) < RADIUS*RADIUS){
                        toIndex = i;
                    }
                }
                //TODO: draw the head of the arrow, finalize

                connect_nodes(G.nodes[fromIndex],G.nodes[toIndex], 1);
                return;
            }
            else
            {   //TODO clicking negative space should stop the edge creating process
                lines[1].position = lines[0].position;
                return;
            }
        }
        draw_board(G, ui_sketches);
        window.draw(lines); // DRAW THE NEW LINES
        window.display();
    }
}

bool isMouseOnAnyNode(Graph G){
    sf::Vector2f mouse_pos = (sf::Vector2f)sf::Mouse::getPosition(window);
    for(auto i : G.nodes){
        if((i.position.x - mouse_pos.x)*(i.position.x - mouse_pos.x) + (i.position.y - mouse_pos.y)*(i.position.y  - mouse_pos.y) < RADIUS*RADIUS){
            return true;
        }
    }
    return false;
}

void changeEdge(Node& toNode, Node& fromNode);
bool editEdgeOnMouse(Graph& G){
    sf::Vector2f mouse_pos = (sf::Vector2f)sf::Mouse::getPosition(window);

    for(auto fromNode : G.nodes){
        for (auto j : fromNode.connectedTo) {
            Node toNode;
            for(auto k : G.nodes){
                if(k.name[0] == j.first[0]){
                    toNode = k; //Finds the node that is connected
                }
            }

            //PART 1
            float x1 = fromNode.position.x;
            float y1 = fromNode.position.y;
            float x2 = toNode.position.x;
            float y2 = toNode.position.y;

            float m = (y1 - y2)/(x1 - x2);
            float c = y1 - (m * x1);

            //PART 2
            float x3 = mouse_pos.x;
            float y3 = mouse_pos.y;
            float b = y3 - (-x3/m);

            float x = (b-c)*m/(m*m + 1);
            float y = -x/m + b;

            //PART 3
            bool dikGorunmezOlmayanYerde = ((x1<=x && x<=x2)||(x2<=x && x<=x1))&&((y1<=y && y<=y2)||(y2<=y && y<=y1));

            if(DEBUGMODE) { // DEBUG MODE
                sf::RectangleShape rectangle(sf::Vector2f(30,30));
                rectangle.setOrigin(15,15);
                rectangle.setPosition(sf::Vector2f(x1,m*x1 + c));
                //cout << y1 << "= y1 ve m= " << m<< ", x= " << x1 << ", c=" << c << ", m * x1 + c=" << m*x1 + c << endl;
                rectangle.setFillColor(sf::Color::Cyan);
                window.draw(rectangle);

                sf::RectangleShape magRectangle(sf::Vector2f(30,30));
                magRectangle.setPosition(sf::Vector2f(x2,m*x2 + c));
                magRectangle.setFillColor(sf::Color::Magenta);
                window.draw(magRectangle);

                sf::VertexArray testLine(sf::LinesStrip,2);
                testLine[0].position = mouse_pos;
                testLine[1].position = sf::Vector2f(x, y);

                testLine[0].color = sf::Color::White;
                testLine[1].color = sf::Color::White;
                window.draw(testLine);
                if (!dikGorunmezOlmayanYerde){
                    testLine[0].color = sf::Color::Red;
                    testLine[1].color = sf::Color::Red;
                    window.draw(testLine);
                    sf::VertexArray extendedRedLine(sf::LinesStrip,2);
                    testLine[0].position = sf::Vector2f(x2,x2*m + c);
                    window.draw(testLine);
                }
            }

            if(((x3-x)*(x3-x)+(y3-y)*(y3-y) < 100) && dikGorunmezOlmayanYerde){
                changeEdge(toNode, fromNode);
                for (int i = 0; i < G.nodes.size(); ++i) {
                    if(G.nodes[i].name == fromNode.name){
                        G.nodes[i].connectedTo[toNode.name] = fromNode.connectedTo[toNode.name];
                    }
                }
                return true;
            }
            window.display();
        }
    }
    return false;
}

void changeEdge(Node& toNode, Node& fromNode){
    sf::Event event;

    while(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
        //cout << "waiting for you to let go of m1 "<< endl;
    }

    sf::VertexArray line(sf::LinesStrip, 2);
    line[0].position = fromNode.position;
    line[1].position = toNode.position;
    line[0].color = sf::Color::Yellow;
    line[1].color = sf::Color::Yellow;
    window.draw(line);
    line[0].position += sf::Vector2f(1,1);
    line[1].position += sf::Vector2f(1,1);
    window.draw(line);
    window.display();

    int newDigit = 0;
    bool kir = false;
    while (!kir) {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){ //İptal etmek için
            kir = true;
        }
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::TextEntered) {
                char key = event.text.unicode;
                if (key >= '0' && key <= '9') {
                    int input = key - '0';
                    newDigit = newDigit * 10 + input;
                } else {
                    kir = true;
                }
                printf(" %d \n", newDigit);
            }
        }
    }

    if(newDigit != 0)
        fromNode.connectedTo[toNode.name] = newDigit;

    cout << endl << " edit is over. New value= " << fromNode.connectedTo[toNode.name] << endl;
}

void draw_board(Graph G, const ui_sketches& ui_sketch){
    window.clear();

    sf::Text my_Text;
    my_Text.setFont(roboto_font);
    my_Text.setCharacterSize(RADIUS/2);
    my_Text.setFillColor(sf::Color::White);

    sf::VertexArray konsol(sf::LinesStrip, 2);
    konsol[0].position.x = 0;
    konsol[0].position.y = 700;
    konsol[1].position.x = 2000;
    konsol[1].position.y = 700;
    konsol[0].color = sf::Color::Green;
    konsol[1].color = sf::Color::Green;
    window.draw(konsol);
    my_Text.setPosition(15,705);
    my_Text.setString("Konsol");
    window.draw(my_Text);

    if(G.nodes.size() > 2) {
        my_Text.setPosition(15, 730);
        unordered_map<string, int> cevap = Dijkstra(G, G.nodes[G.startNodeIndex]);
        string a = "";
        for (auto i: cevap){
            if(i.second == INT_MAX - 1)
                a += "From " + G.nodes[G.startNodeIndex].name + ", shortest to "+ i.first + ", is:  " + "UNREACHABLE" + "\n";
            else
                a += "From " + G.nodes[G.startNodeIndex].name + ", shortest to "+ i.first + ", is:  " + to_string(i.second) + "\n";
        }
        my_Text.setString(a);
        window.draw(my_Text);
    }

    for(auto node : G.nodes){ // Better way to draw the edges
        for(auto i : node.connectedTo){
            sf::VertexArray oldline(sf::LinesStrip, 2);
            oldline[0].position = node.position;
            for(auto j : G.nodes){
                if(j.name[0] == i.first[0]){
                    oldline[1].position = j.position;
                    oldline[1].color = sf::Color::Red;

                    my_Text.setString(to_string(node.connectedTo[j.name]));
                }
            }
            int x1 = (int)oldline[0].position.x;
            int x2 = (int)oldline[1].position.x;
            int y1 = (int)oldline[0].position.y;
            int y2 = (int)oldline[1].position.y;
            my_Text.setPosition(::abs(x1 - x2)/2 + min(x1,x2),::abs(y1 - y2)/2 + min(y1,y2) + 5) ;
            window.draw(my_Text);
            window.draw(oldline);
        }
    }

    for(auto i : G.nodes){ // DRAWING THE NODES
        my_Text.setString(i.name);
        my_Text.setPosition(i.position.x - 5,i.position.y + RADIUS + 10);
        window.draw(my_Text);

        sf::CircleShape newnode(RADIUS,35);
        newnode.setOrigin(RADIUS,RADIUS);
        newnode.setPosition(i.position.x, i.position.y);
        if(i.name == G.nodes[G.startNodeIndex].name)
            newnode.setFillColor(sf::Color::Yellow);
        window.draw(newnode);
    }

    for(auto i : ui_sketch.lines){
        window.draw(i);
    }
}

int main() {
    roboto_font.loadFromFile(R"(C:\Users\deniz\CLionProjects\Dijkstra\Roboto\Roboto-Regular.ttf)");
    Graph G;
    ui_sketches ui_sketches;


    cout << "E->Number edges, F->Clear sketches, Space->Select for Dijkstra, Escape->Close, M1->Create a node or an edge, M2->sketch" << endl;

    while(window.isOpen()){
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            switch(evnt.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    printf("New window width: %d, height: %d \n", evnt.size.width, evnt.size.height);
                    break;
            }
            draw_board(G,ui_sketches);
            window.display();
        }

        // https://www.sfml-dev.org/tutorials/2.4/graphics-vertex-array.php#primitive-types
        sf::Vector2f oldMousePos = (sf::Vector2f) sf::Mouse::getPosition(window);
        sf::VertexArray lines (sf::LinesStrip, 2);
        while(sf::Mouse::isButtonPressed(sf::Mouse::Right)){  // SKETCHING WITH RECTANGLES!
            sf::Vector2f newMousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
            lines[0].position.x = oldMousePos.x;
            lines[0].position.y = oldMousePos.y;
            lines[1].position.x = newMousePos.x;
            lines[1].position.y = newMousePos.y;
            if(lines[0].position.x == lines[1].position.x && lines[0].position.y == lines[1].position.y) // Başlangıçla bitiş aynı yerdeyse çizgi oluşmaz!
                lines[1].position.y--;

            ui_sketches.lines.push_back(lines);
            oldMousePos = newMousePos;
            draw_board(G, ui_sketches);
            window.display();
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::F))
            ui_sketches.lines.clear();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
            if(isMouseOnAnyNode(G)){
                for (int i = 0; i < G.nodes.size(); ++i) {
                    if((G.nodes[i].position.x - oldMousePos.x)*(G.nodes[i].position.x - oldMousePos.x) + (G.nodes[i].position.y - oldMousePos.y)*(G.nodes[i].position.y - oldMousePos.y) < RADIUS*RADIUS){
                        G.startNodeIndex = i;
                    }
                }
            }
        }

        bool isntHeld;
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            if(isntHeld) {
                isntHeld = false;
                if (isMouseOnAnyNode(G)) {
                    ui_create_edge(G, ui_sketches);
                }
                else if(!editEdgeOnMouse(G)){
                    ui_create_node(G);
                }
            }
        }
        else
        {
            isntHeld = true;
        }
    }

    cout << " code has finished";
    return 0;
}
