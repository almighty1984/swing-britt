import app;
import config;
import state;

int main() {
    Config::load("main.cfg");
    App app(320, 180, Config::scale(), "Swing-Britt");
    app.add_state(Config::state_type());
    app.add_state(state::Type::OVERLAY);    
    app.run();
    return 0;
}