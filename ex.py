import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.widgets import CheckButtons

# Sample function to draw rectangles on the given axis
def draw_rectangles(ax):
    """Function to draw rectangles on the given axis."""
    ax.set_title('Rectangles Example')
    for i in range(5):
        ax.add_patch(Rectangle((i, i), 1, 1, alpha=0.5, label=f'Rectangle {i+1}'))
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    ax.legend()

# Create a figure and axis
fig, ax = plt.subplots()
plt.subplots_adjust(left=0.3)  # Adjust to make space for the checkboxes

# Initial drawing of rectangles
draw_rectangles(ax)

# Create checkboxes
ax_checkbox = plt.axes([0.05, 0.4, 0.2, 0.15])
check = CheckButtons(ax_checkbox, ['Redraw Rectangles'], [True])

# Callback function for checkbox click
def update(label):
    if label == 'Redraw Rectangles':
        ax.clear()  # Clear the axis (keeps the axis itself intact)
        draw_rectangles(ax)  # Redraw rectangles on the cleared axis
        plt.draw()  # Refresh the figure with updated content

# Connect the callback function to the checkboxes
check.on_clicked(update)

# Display the plot
plt.show()