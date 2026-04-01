# Copyright (C) 2023 Rémy Cases
# See LICENSE file for extended copyright information.
# This file is part of CFR_simultaneous project from https://github.com/remyCases/CFR_simultaneous.

from collections.abc import Callable

from matplotlib.axes import Axes
from matplotlib.container import BarContainer
from matplotlib.patches import Rectangle
import matplotlib.pyplot as plt
import numpy as np
from numpy.typing import NDArray
import pandas as pd

import matplotlib.animation as animation

def prepare_animation(ax: Axes, pd_data: pd.DataFrame, bar_container: BarContainer) -> Callable[[int], list[Rectangle]]:
    def animate(frame_number: int) -> list[Rectangle]:
        # simulate new data coming in
        n = pd_data.iloc[frame_number, :].to_numpy()
        ax.set_title(f"Average probability of strategies\nstep {frame_number}")
        for val, rect in zip(n, bar_container.patches):
            rect.set_height(val)

        return bar_container.patches
    return animate


def main() -> None:
    pd_data = pd.read_csv("export_avg_strategy.csv", sep=';')
    nb_frames: int = pd_data.shape[0] if pd_data.shape[0] < 100 else 100
    frames: NDArray[np.int64] = np.linspace(0, pd_data.shape[0], num=nb_frames, dtype=int, endpoint=False)
    fig, ax = plt.subplots()
    bar_container: BarContainer = ax.bar(
        pd_data.columns,
        pd_data.iloc[0,:],
        ec="yellow",
        fc="green",
        alpha=0.5
    )
    ax.set_ylim(0.4)
    ax.invert_yaxis()

    ani = animation.FuncAnimation(fig, prepare_animation(ax, pd_data, bar_container), frames, repeat=False, blit=True)

    writervideo = animation.PillowWriter(fps=60)
    ani.save('histo.gif', writer=writervideo)
    plt.close()

if __name__ == "__main__":
    main()