from hex import *
from keras.models import model_from_json

COUNT = 0


class ModelTest:
    def __init__(self):
        json_file = open('E:/Project/.Github/HexKing/Test_1/py/b1_173.json', 'r')
        loaded_model_json = json_file.read()
        json_file.close()
        self.model = model_from_json(loaded_model_json)
        self.model.load_weights("E:/Project/.Github/HexKing/Test_1/py/b1_173.h5")
        # print("init successful")

    def test_state(self, info: [bool, (int, int), (int, int)], state: [[]]) -> []:
        # print(info, state)
        # return []
        global COUNT
        board = HexBoard()
        for row in range(11):
            for col in range(11):
                if state[row][col] == 0:
                    board.set_chess(Player.black, (row, col))
                elif state[row][col] == 1:
                    board.set_chess(Player.white, (row, col))
        sequence = NUM[COUNT]
        COUNT += 1 if COUNT < 119 else -119
        pre_black = copy.deepcopy(board.state[Player.black])
        pre_white = copy.deepcopy(board.state[Player.white])
        if info[2 - info[0]] != (255, 255):
            move = info[2 - info[0]]
            pre_black[move[0]][move[1]] = 0
        if info[1 + info[0]] != (255, 255):
            move = info[1 + info[0]]
            pre_white[move[0]][move[1]] = 0
        tensor = [
            board.state[Player.black],
            board.state[Player.white],
            pre_black,
            pre_white,
            np.ones((11, 11)) if info[0] else np.zeros((11, 11)),  # 0:black
            np.zeros((11, 11)) if info[0] else np.ones((11, 11)),  # 0:black

        ]
        inputs = np.stack([np.stack([
            tensor[sequence[0]],
            tensor[sequence[1]],
            tensor[sequence[2]],
            tensor[sequence[3]],
            tensor[sequence[4]]
        ])])
        policy_0, policy_1, value = self.model.predict(inputs)
        return value[0].tolist() + policy_0[0].tolist() + policy_1[0].tolist()


def test_model():
    test = ModelTest()
    game = HexGame()
    game.set_move((1, 1))
    game.set_move((1, 2))
    info = [False, (1, 2), (1, 1)]
    state = []
    for row in range(11):
        state.append([])
        for col in range(11):
            if game.board.stones.get((row, col)) is None:
                state[row].append(2)
            elif game.board.stones.get((row, col)).player == Player.black:
                state[row].append(0)
            else:
                state[row].append(1)
    print(len(test.test_state(info, state)))



# 97 98 102 104 108 109 115 118
NUM = [
    [0, 1, 2, 3, 4],
    [0, 1, 2, 4, 3],
    [0, 1, 3, 2, 4],
    [0, 1, 3, 4, 2],
    [0, 1, 4, 2, 3],
    [0, 1, 4, 3, 2],
    [0, 2, 1, 3, 4],
    [0, 2, 1, 4, 3],
    [0, 2, 3, 1, 4],
    [0, 2, 3, 4, 1],
    [0, 2, 4, 1, 3],
    [0, 2, 4, 3, 1],
    [0, 3, 1, 2, 4],
    [0, 3, 1, 4, 2],
    [0, 3, 2, 1, 4],
    [0, 3, 2, 4, 1],
    [0, 3, 4, 1, 2],
    [0, 3, 4, 2, 1],
    [0, 4, 1, 2, 3],
    [0, 4, 1, 3, 2],
    [0, 4, 2, 1, 3],
    [0, 4, 2, 3, 1],
    [0, 4, 3, 1, 2],
    [0, 4, 3, 2, 1],
    [1, 0, 2, 3, 4],
    [1, 0, 2, 4, 3],
    [1, 0, 3, 2, 4],
    [1, 0, 3, 4, 2],
    [1, 0, 4, 2, 3],
    [1, 0, 4, 3, 2],
    [1, 2, 0, 3, 4],
    [1, 2, 0, 4, 3],
    [1, 2, 3, 0, 4],
    [1, 2, 3, 4, 0],
    [1, 2, 4, 0, 3],
    [1, 2, 4, 3, 0],
    [1, 3, 0, 2, 4],
    [1, 3, 0, 4, 2],
    [1, 3, 2, 0, 4],
    [1, 3, 2, 4, 0],
    [1, 3, 4, 0, 2],
    [1, 3, 4, 2, 0],
    [1, 4, 0, 2, 3],
    [1, 4, 0, 3, 2],
    [1, 4, 2, 0, 3],
    [1, 4, 2, 3, 0],
    [1, 4, 3, 0, 2],
    [1, 4, 3, 2, 0],
    [2, 0, 1, 3, 4],
    [2, 0, 1, 4, 3],
    [2, 0, 3, 1, 4],
    [2, 0, 3, 4, 1],
    [2, 0, 4, 1, 3],
    [2, 0, 4, 3, 1],
    [2, 1, 0, 3, 4],
    [2, 1, 0, 4, 3],
    [2, 1, 3, 0, 4],
    [2, 1, 3, 4, 0],
    [2, 1, 4, 0, 3],
    [2, 1, 4, 3, 0],
    [2, 3, 0, 1, 4],
    [2, 3, 0, 4, 1],
    [2, 3, 1, 0, 4],
    [2, 3, 1, 4, 0],
    [2, 3, 4, 0, 1],
    [2, 3, 4, 1, 0],
    [2, 4, 0, 1, 3],
    [2, 4, 0, 3, 1],
    [2, 4, 1, 0, 3],
    [2, 4, 1, 3, 0],
    [2, 4, 3, 0, 1],
    [2, 4, 3, 1, 0],
    [3, 0, 1, 2, 4],
    [3, 0, 1, 4, 2],
    [3, 0, 2, 1, 4],
    [3, 0, 2, 4, 1],
    [3, 0, 4, 1, 2],
    [3, 0, 4, 2, 1],
    [3, 1, 0, 2, 4],
    [3, 1, 0, 4, 2],
    [3, 1, 2, 0, 4],
    [3, 1, 2, 4, 0],
    [3, 1, 4, 0, 2],
    [3, 1, 4, 2, 0],
    [3, 2, 0, 1, 4],
    [3, 2, 0, 4, 1],
    [3, 2, 1, 0, 4],
    [3, 2, 1, 4, 0],
    [3, 2, 4, 0, 1],
    [3, 2, 4, 1, 0],
    [3, 4, 0, 1, 2],
    [3, 4, 0, 2, 1],
    [3, 4, 1, 0, 2],
    [3, 4, 1, 2, 0],
    [3, 4, 2, 0, 1],
    [3, 4, 2, 1, 0],
    [4, 0, 1, 2, 3],
    [4, 0, 2, 1, 3],
    [4, 0, 2, 3, 1],
    [4, 0, 1, 3, 2],
    [4, 0, 3, 1, 2],
    [4, 0, 3, 2, 1],
    [4, 1, 0, 2, 3],
    [4, 1, 0, 3, 2],
    [4, 1, 2, 0, 3],
    [4, 1, 2, 3, 0],
    [4, 1, 3, 0, 2],
    [4, 1, 3, 2, 0],
    [4, 2, 0, 1, 3],
    [4, 2, 0, 3, 1],
    [4, 2, 1, 0, 3],
    [4, 2, 1, 3, 0],
    [4, 2, 3, 0, 1],
    [4, 2, 3, 1, 0],
    [4, 3, 0, 1, 2],
    [4, 3, 0, 2, 1],
    [4, 3, 1, 0, 2],
    [4, 3, 1, 2, 0],
    [4, 3, 2, 0, 1],
    [4, 3, 2, 1, 0]
]

def test():
    # print(NUM[97],NUM[98],NUM[102],NUM[104],NUM[108],NUM[109],NUM[115],NUM[118])
    print(NUM[43], NUM[45])
if __name__ == '__main__':
    # test_model()
    test()