
import logging
import game as az
from game.hex import HexGame
import re
import time


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
        self.who_win = None
        self.attacker = True  # True: black
        self.history = []

    def action(self, cmd: int):
        # M: move 1-121  C: computer 0  U: undo <0  S: surrender 200
        if self.who_win is not None:
            return None
        if cmd == 0:
            move = self.agent.choose_action()
            result = self.agent.execute_action(move)
            self.history.append(move)
            if result:
                self.who_win = self.attacker
            self.attacker = not self.attacker
            return move
        elif cmd < 0:
            step_num = -cmd
            if step_num > len(self.history):
                return -200
            self.history = self.history[:cmd]
            self.agent.reset()
            for move in self.history:
                self.agent.execute_action(move)
            # if step_num > len(self.history):
            #     return -1
            # self.agent.undo()
            return self.history[-1] if len(self.history) else None
        elif cmd == 200:
            self.who_win = not self.attacker
            return cmd
        else:
            if cmd in self.history:
                return
            self.history.append(cmd)
            assert 1 <= cmd <= 121, "coord error!"
            result = self.agent.execute_action(cmd)
            if result:
                self.who_win = self.attacker
            self.attacker = not self.attacker
            return cmd


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


def game():
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        datefmt='%Y-%m-%d %H:%M:%S')

    cpp_run = CppRun('')

    def print_board(mv):
        az.human_agent.print_board(cpp_run.agent.game.io, cpp_run.agent.game.state.board, mv)

    def cmd(move_txt):
        if move_txt == 'AI' or move_txt == 'ai':
            return 0
        elif move_txt == 'S' or move_txt == 's':
            return 200
        else:
            x = move_txt[0]
            if x == 'u' or x == 'U':
                return int(move_txt[1:]) if move_txt[2:] != 'all' \
                    else (-len(cpp_run.history) if len(cpp_run.history) else -1)
            else:
                row = int(move_txt[1:]) - 1
                col = ord(x) - ord('a')
                return (10 - row) * 11 + col + 1

    pattern = (
        r"\bS\b|\bs\b"
        r"|\bAI\b|\bai\b"
        r"|\bu-([1-9]\d*|all)\b|\bU-([1-9]\d*|all)\b"
        r"|\b[a-k]([1-9]|10|11)\b|\b[A-K]([1-9]|10|11)\b"
    )

    move = None
    while True:
        print_board(move)
        while True:
            ipt = input(('Blue' if len(cpp_run.history) & 1 else 'Red') + ' - CMD> ')
            if re.match(pattern, ipt):
                start = time.time()
                move = cpp_run.action(cmd(ipt))
                if ipt == 'ai' or ipt == 'AI':
                    print(f'used time : {time.time() - start:.2f}')
                if move == -200:
                    print('undo failed : history less than steps !')
                    continue
                break
        if move == 200:
            print(f"winner is {'red' if cpp_run.who_win else 'blue'}")
            break
        if cpp_run.who_win is not None:
            print_board(move)
            print(f"winner is {'red' if cpp_run.who_win else 'blue'}")
            break
    print('End')


if __name__ == '__main__':
    game()
