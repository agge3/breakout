#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <boost/foreach.hpp>

using namespace sf;

// constexpr defines an immutable compile-time value
constexpr int window_width{800}, window_height{600};

// constants for the Ball class
constexpr float ball_radius{10.f}, ball_velocity{8.f};

// create some constants for paddle (player)
constexpr float player_width{60.f}, player_height{20.f}, player_velocity{8.f};

// constants for bricks to be destroyed
constexpr float block_width(60.f), block_height{20.f};
constexpr int count_blocks_x{11}, count_blocks_y{4};

struct Ball {
    CircleShape shape;

    // 2D vector that stores the Ball's velocity
    // stores 2 floats
    Vector2f velocity{-ball_velocity, -ball_velocity};

    // constructor
    Ball(float m_x, float m_y) {
        // instructions to happen at instruction time
        shape.setPosition(m_x, m_y);
        shape.setRadius(ball_radius);
        shape.setFillColor(Color::Red);
        // want to be center of the shape
        shape.setOrigin(ball_radius, ball_radius);
    }

    // "update" the ball: move its shape, by the current velocity
    // common and "best" way. every game loop, change obj pos by velocity
    // "illusion" of movement
    void update()
    {
        shape.move(velocity);

        // find a way to keep the ball "inside the screen"

        // check if ball is outside of window and flip velocity (on update
        // cycle)

        // if it's leaving toward the left, we need to set horizontal velocity
        // to a positive value (torwards the right)
        if (left() < 0)
            velocity.x = ball_velocity;

        // if it's leaving torwards the right, we need to set horizontal
        // velocity to a negative value (torwards the left)
        else if (right() > window_width)
            velocity.x = -ball_velocity;

        // same for top and bottom collisions
        if (top() < 0)
            velocity.y = ball_velocity;
        else if (bottom() > window_height)
            velocity.y = -ball_velocity;
    }

    // "property" methods to easily get commonly used values
    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getRadius(); }
    float right() { return x() + shape.getRadius(); }
    float top() { return y() - shape.getRadius(); }
    float bottom() { return y() + shape.getRadius(); }
};

struct Player {
    RectangleShape shape;
    Vector2f velocity;

    // constructor for player with arguments for initial position and pass the
    // values to SFML 'shape'
    Player(float m_x, float m_y) { // takes x & y starting pos
        shape.setPosition(m_x, m_y);
        shape.setSize({player_width, player_height});
        shape.setFillColor(Color::Red);
        shape.setOrigin(player_width / 2.f, player_height / 2.f);
    }

    void update()
    {
        shape.move(velocity);

        // to move the paddle, we check if the user is pressing the left or
        // right arrow key: if so, we change the velocity

        // to keep the paddle "inside the window", we change the velocity only
        // if its position is inside the window
        // if the command sends player out of window, not able to input!
        if (Keyboard::isKeyPressed(Keyboard::Key::Left) && left() > 0)
            velocity.x = -player_velocity;
        else if (Keyboard::isKeyPressed(Keyboard::Key::Right)
                && right() < window_width)
            velocity.x = player_velocity;

        // if the user isn't pressing anything, stop moving
        else
            velocity.x = 0;
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};

struct Brick {
    RectangleShape shape;

    // check whether the brick has been destroyed
    bool destroyed{false};

    Brick(float m_x, float m_y) {
        shape.setPosition(m_x, m_y);
        shape.setSize({block_width, block_height});
        shape.setFillColor(Color::Yellow);
        shape.setOrigin(block_width / 2.f, block_height / 2.f);
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};

// dealing with collisions: let's define a generic function to check if two
// shapres are intersecting (colliding)
template<typename T1, typename T2>
bool is_intersecting(T1& m_a, T2& m_b)
{
    // if a side of the obj is greater or equal to another side, it MUST be
    // intersecting
    return m_a.right() >= m_b.left() && m_a.left() <= m_b.right() // hori inter
        && m_a.bottom() >= m_b.top() && m_a.top() <= m_b.bottom(); // vert inte
}

// function to deal with paddle/ball collision
void test_collision(Player& m_player, Ball& m_ball)
{
    // if there's not interection don't handle, return void
    if (is_intersecting(m_player, m_ball)) {
        // push the ball upwards
        m_ball.velocity.y = -ball_velocity;
        // and let's direct it, dependent on the position where it was hit
        // if hit on right side, push to right
        // if hit on left side, push to left
        // check where on x-axis to determine
        if (m_ball.x() < m_player.x())
            m_ball.velocity.x = -ball_velocity;
        else
            m_ball.velocity.x = ball_velocity;
    }
}

void test_collision(Brick& m_brick, Ball& m_ball)
{
    if (is_intersecting(m_brick, m_ball)) {
        // the brick has been hit! destroy
        m_brick.destroyed = true;

        // calculate how much the ball intersects the brick in every direction
        float overlap_left{m_ball.right() - m_brick.left()};
        float overlap_right{m_brick.right() - m_ball.left()};
        float overlap_top{m_ball.bottom() - m_brick.top()};
        float overlap_bottom{m_brick.bottom() - m_ball.top()};
        // depending on what side is hit, one side or the other will have a
        // bigger difference!

        // if the magnitude of the left overlap is smaller than the right one,
        // we can safely assume the ball hit the brick from the left
        bool ball_from_left(abs(overlap_left) < abs(overlap_right));

        // we can safely assume same idea for top/bottom collisions
        bool ball_from_top(abs(overlap_top) < abs(overlap_top));

        // store the minimum overlaps for the X and Y axes
        float min_overlap_x{ball_from_left ? overlap_left : overlap_right};
        float min_overlap_y{ball_from_top ? overlap_top : overlap_bottom};

        // if the magnitude of the X overlap is less than the magnitude of the
        // Y overlap, we can safely assume the ball hit the brick horizontally.
        // Otherwise, the ball hit the brick vertically

        // based on these assumptions, we can change the X or Y velocity of the
        // ball, creating a "realistic" response for the collision
        if (abs(min_overlap_x) < abs(min_overlap_y))
            m_ball.velocity.x = ball_from_left ? -ball_velocity : ball_velocity;
        else
            m_ball.velocity.y = ball_from_top ? -ball_velocity : ball_velocity;
    }
}

int main()
{
    // create an instance of Ball positioned at the center of the window
    // positioned at center of the window
    Ball ball{window_width / 2, window_height / 2};

    // create player instance
    Player player{window_width / 2, window_height - 50};

    // vector to contain any number of 'brick' instances
    std::vector<Brick> bricks;

    // fill up vector via a 2D for loop, creating bricks in a grid-like pattern
    for (int i_x{0}; i_x < count_blocks_x; ++i_x) // 0 to num of cols
        for (int i_y{0}; i_y < count_blocks_y; ++i_y) // 0 to num of rows
            // emplace is cpp 11, allows to directly construct obj inside of a
            // container = more efficient & readable
            bricks.emplace_back((i_x + 1) * (block_width + 3) + 22,
                    (i_y + 2) * (block_height + 3));

    // creation of the game window
    RenderWindow window{{window_width, window_height}, "Practice Game"};
    window.setFramerateLimit(60);

    // game loop
    while (window.isOpen()) {
        // clear the window from previously drawn graphics
        window.clear(Color::Black);

        // if escape is pressed, break out of the loop
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
            break;

        // every game loop, update the ball
        ball.update();
        // update the player
        player.update();

        // test the collision every loop itteration
        test_collision(player, ball);

        // foreach loop to check collisions between the ball and EVERY brick
        for (auto& brick : bricks)
            test_collision(brick, ball);

        // stl algo to remove all 'destroyed' bricks from the brick vector
        // cpp 11 lambda!'
        // bricks.erase(start_iterator, end_iterator);
        // remove_if, takes a range and predicate (func or lambda)
        // remove_if all 'bricks' from the brick' vector if 'destroyed'
        // remove_if returns an iterator
        // erase it taking remove_if's return iterator for beginning (i.e.,
        // beginning of destroyed
        // vector will not get modified multiple times, if multiple bricks are
        // destroyed... efficient!
        bricks.erase(remove_if(begin(bricks), end(bricks),
                [] (const Brick& m_brick) { return m_brick.destroyed; }),
                end(bricks));

        // show the window contents
        // render the ball instance on the window
        window.draw(ball.shape); // draw function of window. is obj drawable?
        window.draw(player.shape);

        // must draw every brick on the window!
        // cpp 11 foreach loop
        // allows us to say "draw every 'brick' in 'bricks'"
        // for every reference to 'brick' in 'bricks' draw...
        // want to use ref, don't create a copy!
        for (auto& brick : bricks)
            window.draw(brick.shape);

        window.display();
    }

    return 0;
}
