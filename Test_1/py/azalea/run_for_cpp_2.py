import azalea as az
from azalea.game.hex import HexGame

AGENT = 0



class CppRun:
    def __init__(self, path):
        if AGENT == 0:
            AGENT = az.AzaleaAgent(
                HexGame,
                path='{0}{1}models/hex11-20180712-3362.policy.pth'
                .format(path, ('' if len(path) == 0
                               else ('/' if path[-1] != '/' and path[-1] != '\\' else '')))
            )
            self.agnet = AGENT
        self.agent.seed(0)
        self.agent.reset()
        self.is_end = False
        self.step_num = 0

    def action(self, cmd: int):
        # M: move 1-121  A: agent 0  U: undo -1  S: surrender -2
        if self.is_end:
            return None
        if not cmd:
            self.step_num += 1
            move = self.agent.choose_action()
            result = self.agent.execute_action(move)
            if result:
                self.is_end = True
            return move
        elif cmd == -1:
            if self.step_num == 0:
                return None
            self.step_num -= 1
            self.agent.undo()
        elif cmd == -2:
            self.is_end = True
        else:
            self.step_num += 1
            assert 1 <= cmd <= 121, "coord error!"
            result = self.agent.execute_action(cmd)
            if result:
                self.is_end = True
        return None


def test():
    # ha = az.HumanAgent()
    cpp_run = CppRun('')
    for i in range(2):
        move = cpp_run.action(0)
        az.human_agent.print_board(cpp_run.agent.game.io, cpp_run.agent.game.state.board, move)
    for i in range(3):
        move = -1
        cpp_run.action(move)
        az.human_agent.print_board(cpp_run.agent.game.io, cpp_run.agent.game.state.board, move)
        move = cpp_run.action(0)
        az.human_agent.print_board(cpp_run.agent.game.io, cpp_run.agent.game.state.board, move)
    move = 12
    cpp_run.action(move)
    az.human_agent.print_board(cpp_run.agent.game.io, cpp_run.agent.game.state.board, move)
    move = -1
    cpp_run.action(move)
    az.human_agent.print_board(cpp_run.agent.game.io, cpp_run.agent.game.state.board, move)
    print("PASS")


if __name__ == '__main__':
    test()
