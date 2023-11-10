import azalea as az
from azalea.game.hex import HexGame


class CppRun:
    def __init__(self, path):
        self.agent = az.AzaleaAgent(
            HexGame,
            path='{0}{1}models/hex11-20180712-3362.policy.pth'
            .format(path, ('' if len(path) == 0
                           else ('/' if path[-1] != '/' and path[-1] != '\\' else '')))
        )
        self.agent.seed(0)
        self.agent.reset()
        self.is_end = False
        self.history = []

    def action(self, cmd: int):
        # M: move 1-121  A: agent 0  U: undo <0  S: surrender 200
        if self.is_end:
            return
        if cmd == 0:
            move = self.agent.choose_action()
            result = self.agent.execute_action(move)
            self.history.append(move)
            if result:
                self.is_end = True
            return move
        elif cmd < 0:
            step_num = -cmd
            assert step_num <= len(self.history)
            self.history = self.history[:cmd]
            self.agent.reset()
            for move in self.history:
                self.agent.execute_action(move)
            # if step_num > len(self.history):
            #     return -1
            # self.agent.undo()
        elif cmd == 200:
            self.is_end = True
        else:
            self.history.append(cmd)
            assert 1 <= cmd <= 121, "coord error!"
            result = self.agent.execute_action(cmd)
            if result:
                self.is_end = True
        return


def test():
    # ha = az.HumanAgent()
    cpp_run = CppRun('')
    for i in range(4):
        move = cpp_run.action(0)
        az.human_agent.print_board(cpp_run.agent.game.io, cpp_run.agent.game.state.board, move)
    move = -3
    cpp_run.action(move)
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
