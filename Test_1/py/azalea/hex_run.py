import numpy as np
import torch
# import azalea as az


class MoveForCpp:
    def __init__(self):
        model_path = 'models/hex11-20180712-3362.policy.pth'
        config = {
            'game': 'hex',
            'seed': 0xBAD5EED5,
            'device': 'cuda' if torch.cuda.is_available() else 'cpu'
        }

        np.random.seed(config['seed'])
        torch.manual_seed(config['seed'])
        torch.cuda.manual_seed(config['seed'])

        self.policy = az.Policy.load(config, model_path)
        config['board_size'] = self.policy.board_size

        self.gamelib = az.import_game(config['game'])
        # new game
        self.game = self.gamelib.GameState(board_size=config['board_size'])
        self.node = self.policy.reset(self.game)
        self.m = 0

    def reset(self):
        self.game = self.gamelib.GameState(board_size=self.policy.board_size)
        self.node = self.policy.reset(self.game)
        self.m = 0

    def action(self, flag, act):
        if flag == -1:
            self.reset()
        if flag == -1 or flag == 1:
            for i in range(len(act)):
                moves = self.game.legal_moves()
                index = self.gamelib.parse_move(self.game.board(), 0, act[i])
                action = list(moves).index(index)
                self.game.move(moves[action])
                self.node = self.policy.make_action(self.game, self.node, action, moves)
                self.m += 1

    def choose_move(self):
        moves = self.game.legal_moves()
        action, self.node, _, _ = \
            self.policy.choose_action(self.game, self.node, self.m,
                                      temperature=0, noise_scale=0)
        self.game.move(moves[action])
        return moves[action]


def print_board(utils, board, computer_move):
    print('\033[2J')
    if computer_move:
        utils.print_board(board)  # , highlight=computer_move)
        print('last move: {}'.format(utils.format_move(
            board, computer_move)))
    else:
        utils.print_board(board)


def test():
    mfc = MoveForCpp()
    mfc.action(1, ['a1'])
    print_board(mfc.gamelib, mfc.game.board(), 1)
    m = mfc.choose_move()
    print_board(mfc.gamelib, mfc.game.board(), m)
    mfc.action(-1, ['e7'])
    print_board(mfc.gamelib, mfc.game.board(), m)


if __name__ == '__main__':
    test()