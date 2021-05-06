import sys
from PyQt5 import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
import JSON_parser as jp

import json

MAIN_WINDOW = None

JSON_FILE = None

SUPPORTED_TYPES = {
    "uint32_t": {"size": 4, "color": (91, 155, 213)},
    "uint16_t": {"size": 2, "color": (167, 202, 233)},
    "uint8_t": {"size": 1, "color": (223, 236, 247)},
    "bool": {"size": 1, "color": (159, 224, 142)},
    "float": {"size": 4, "color": (111, 111, 111)},
    "double": {"size": 8, "color": (111, 111, 111)},
    "char": {"size": 1, "color": (111, 111, 111)},
    "string": {"size": 0, "color": (111, 111, 111)},
}

AMDC_sys_path = "../../sdk/bare/common/sys/"
ethernet_logging_path = "../../sdk/ethernet_logging/src/"

class ICCFileDialog(QWidget):

    def __init__(self, import_f=True):
        super().__init__()
        self.import_f = import_f
        self.title = 'PyQt5 file dialogs - pythonspot.com'
        self.left = 10
        self.top = 10
        self.width = 640
        self.height = 480
        self.initUI()

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)

        if self.import_f:
            self.openFileNameDialog()
        else:
            self.saveFileDialog()



        self.show()

    def openFileNameDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getOpenFileName(self, "QFileDialog.getOpenFileName()", "",
                                                  "All Files (*);;Python Files (*.py)", options=options)
        if fileName:
            global JSON_FILE
            JSON_FILE = fileName

    def saveFileDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "",
                                                  "All Files (*);;Text Files (*.txt)", options=options)
        if fileName:
            global JSON_FILE
            JSON_FILE = fileName


class ICCDataframeRow(QWidget):
    def __init__(self, numAddWidget):
        QWidget.__init__(self)

        self.numAddWidget = numAddWidget
        self.numAddItem = 1

        self.initSubject()

    def initSubject(self):
        self.root = QWidget(self)
        self.line = QLineEdit(self.root)
        self.line.setMinimumSize(QSize(100, 40))
        font = QFont()
        font.setPointSize(10)
        self.line.setFont(font)
        self.line.setPlaceholderText("Variable Name")

        self.combo = QComboBox(self.root)
        for key in SUPPORTED_TYPES:
            self.combo.addItem(key)
        self.combo.currentIndexChanged.connect(self.typeChange)
        self.combo.setMinimumSize(QSize(80, 40))
        self.combo.setMaximumSize(QSize(100, 40))
        # self.combo.setStyleSheet(
        #     "QComboBox{background-color: rgb(%i, %i, %i);}" %
        #     (
        #         SUPPORTED_TYPES[0]["color"][0],
        #         SUPPORTED_TYPES[0]["color"][1],
        #         SUPPORTED_TYPES[0]["color"][2])
        #     )

        # self.val = QLineEdit(self.root)
        # self.val.setMinimumSize(QSize(0, 40))

        self.delete = QPushButton(self.root)
        self.delete.setText("...")
        self.delete.setStyleSheet("QPushButton::menu-indicator{width:0px;}")
        self.delete.setMinimumSize(QSize(40, 40))
        self.delete.setMaximumSize(QSize(40, 40))

        self.hlayout = QHBoxLayout(self)
        self.hlayout.addWidget(self.line)
        self.hlayout.addWidget(self.combo)
        # self.hlayout.addWidget(self.val)
        self.hlayout.addWidget(self.delete)
        self.hlayout.setContentsMargins(0, 0, 0, 0)
        self.hlayout.setSpacing(0)

        self.delete_action = QAction("Delete Row")
        self.insert_row_below = QAction("Insert Row Below")
        self.insert_row_above = QAction("Insert Row Above")

        self.delete_action.triggered.connect(self.clicked)
        self.insert_row_below.triggered.connect(lambda: MAIN_WINDOW.insertWidget(self, below=True))
        self.insert_row_above.triggered.connect(lambda: MAIN_WINDOW.insertWidget(self, below=False))

        self.menu = QMenu(self)
        self.menu.addAction(self.delete_action)
        self.menu.addAction(self.insert_row_below)
        self.menu.addAction(self.insert_row_above)

        self.delete.setMenu(self.menu)
        # self.menu.popup()

    def clicked(self):
        print("CLCIK", self.numAddWidget)

        MAIN_WINDOW.removedWidget(self)

        self.deleteLater()

    def updateID(self, id):
        pass

    def typeChange(self):
        print("changing type")
        type = self.combo.currentText()
        idx = self.combo.currentIndex()

        # self.combo.setStyleSheet(
        #     "QComboBox{background-color: rgb(%i, %i, %i);}" %
        #     (
        #         SUPPORTED_TYPES[idx]["color"][0],
        #         SUPPORTED_TYPES[idx]["color"][1],
        #         SUPPORTED_TYPES[idx]["color"][2])
        #     )


class ICCDataframeApp(QMainWindow):
    """This is the main application window for the AMDC ICC application"""

    def __init__(self):
        super().__init__()
        self.num_rows = 0  # Index for rows
        self.initUi()

        self.menubar = self.menuBar()

        # Creating menus using a QMenu object
        self.file_menu = self.menubar.addMenu("&File")

        self.new_action = self.file_menu.addAction("New Table")
        self.new_action.triggered.connect(self.startTable)

        self.import_action = self.file_menu.addAction("Import JSON")
        self.import_action.triggered.connect(self.importTable)

        self.export_action = self.file_menu.addAction("Export JSON")
        self.export_action.triggered.connect(self.export)

    def initUi(self):
        self.root = QWidget(self)
        self.setCentralWidget(self.root)
        # Resize window to 480 x 640 portrait
        self.resize(480, 640)

        # Main layout for window
        self.v_layout_main = QVBoxLayout(self.root)

        # Scroll Area for dataframe
        self.area = QScrollArea(self.root)
        self.area.setWidgetResizable(True)
        self.area_widget_contents = QWidget()
        self.area.setWidget(self.area_widget_contents)

        # Scroll Area layout
        self.v_layout_area = QVBoxLayout(self.area_widget_contents)
        self.v_layout_area.setSpacing(0)

        # New Table / Import Table Widget
        self.table_widget = QWidget(self.area)
        self.table_widget_layout = QHBoxLayout(self.table_widget)
        self.import_button = QPushButton(self.area)
        self.import_button.setText("Import Table")
        self.import_button.clicked.connect(self.importTable)
        self.table_widget_layout.addWidget(self.import_button)
        self.new_button = QPushButton(self.area)
        self.new_button.setText("New Table")
        self.new_button.clicked.connect(self.startTable)
        self.table_widget_layout.addWidget(self.new_button)
        self.v_layout_area.addWidget(self.table_widget)

        # Add scroll area to main window layout
        self.v_layout_main.addWidget(self.area)

    def addWidget(self):
        self.widget = ICCDataframeRow(self.num_rows)

        self.v_layout_area.insertWidget(self.num_rows, self.widget)
        self.num_rows += 1

    def insertWidget(self, widget, below):
        self.widget = ICCDataframeRow(self.num_rows)
        self.num_rows += 1
        index = self.v_layout_area.indexOf(widget)
        self.v_layout_area.insertWidget(index + 1 if below else index + 0, self.widget)

    def startTable(self):
        # Empty widget to pack scroll area

        print("ADDING PACK WIDGET")
        self.area_pack_widget = QWidget(self.area_widget_contents)
        size_policy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Expanding)
        self.area_pack_widget.setSizePolicy(size_policy)
        self.v_layout_area.addWidget(self.area_pack_widget)

        self.addWidget()

        self.table_widget.deleteLater()

    def importTable(self):
        w = ICCFileDialog()

        if JSON_FILE == None:
            return

        self.table_widget.deleteLater()

        print("ADDING PACK WIDGET")
        self.area_pack_widget = QWidget(self.area_widget_contents)
        size_policy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Expanding)
        self.area_pack_widget.setSizePolicy(size_policy)
        self.v_layout_area.addWidget(self.area_pack_widget)

        print(JSON_FILE)
        print("Opening file")
        with open(JSON_FILE, "r") as self.JSON:
            data = self.JSON.read()
            dict = json.loads(data)
            dataframe = dict["table"]
            for entry in dataframe:
                self.widget = ICCDataframeRow(self.num_rows)
                self.widget.line.setText(entry["name"])
                self.widget.combo.setCurrentText(entry["datatype"])

                self.v_layout_area.insertWidget(self.num_rows, self.widget)
                self.num_rows += 1
                print("%s is type %s" % (entry["name"], entry["datatype"]))


    def removedWidget(self, widget):
        self.num_rows -= 1

    def export(self):
        print("Exporting JSON")

        w = ICCFileDialog(False)
        dataframe = []

        children = self.area_widget_contents.children()
        print(children)

        for child in children:
            if isinstance(child, ICCDataframeRow):
                print("Row found")
                print(child.line.text())
                print(child.combo.currentText())
                var = {"name": child.line.text(),
                       "datatype": child.combo.currentText()}
                dataframe.append(var)

        print(dataframe)
        out_file = open(JSON_FILE, "w+")

        json_obj = {
            "filename": "amdc_icc.json",
            "table": dataframe
        }

        json.dump(json_obj, out_file, indent=2)

        out_file.close()

        generate_c_files(dataframe)


def generate_c_files(dataframe):
    table = dataframe
    with open(AMDC_sys_path + "icc.h", "w+") as fp:
        fp.write("#ifndef ICC_H\n")
        fp.write("#define ICC_H\n\n")

        fp.write("#include <stdbool.h>\n")
        fp.write("#include <stdint.h>\n\n")

        fp.write("#define ICC_CACHE_BASE_ADDR 	0x3000000\n")
        fp.write("#define ICC_CACHE_SIZE		0x100000\n\n")

        table_size = 0
        for i in range(0, len(table)):
            table_size += SUPPORTED_TYPES[table[i]["datatype"]]["size"]

        fp.write("#define ICC_TABLE_SIZE		 " + str(table_size) +"\n\n")

        fp.write("uint8_t* ICC_F;\n\n")

        for i in range(0, len(table)):
            fp.write(table[i]["datatype"] + "* ICC_" + table[i]["name"] + ";\n")

        fp.write("\nvoid icc_init(void);\n")
        fp.write("\n#endif // ICC_H\n")


        fp.close()

    with open(AMDC_sys_path + "icc.c", "w+") as fp:
        fp.write("#include \"usr/user_config.h\"\n")
        fp.write("#include \"sys/icc.h\"\n")

        fp.write("\nvoid icc_init(void)\n{\n")
        fp.write("\tICC_F = (void *)ICC_CACHE_BASE_ADDR;\n")
        fp.write("\t*ICC_F = 0x00;\n")

        fp.write("\n\tuint32_t offset = 5;\n")

        for i in range(0, len(table)):
            fp.write("\n\tICC_" + table[i]["name"] + " = ((void*)ICC_CACHE_BASE_ADDR + offset);\n")
            fp.write("\t*ICC_" + table[i]["name"] + "= 0x0000;\n")
            fp.write("\toffset += sizeof(*ICC_" + table[i]["name"] + ");\n")

        fp.write("\n}")

    with open(ethernet_logging_path + "icc.h", "w+") as fp:
        fp.write("#ifndef ICC_H\n")
        fp.write("#define ICC_H\n\n")

        fp.write("#include <stdbool.h>\n")
        fp.write("#include <stdint.h>\n\n")

        fp.write("#define ICC_CACHE_BASE_ADDR 	0x3000000\n")
        fp.write("#define ICC_CACHE_SIZE		0x100000\n\n")

        fp.write("#define ICC_TABLE_SIZE		0x16\n\n")

        fp.write("uint8_t* ICC_F;\n\n")

        table = dataframe
        for i in range(0, len(table)):
            fp.write(table[i]["datatype"] + "* ICC_" + table[i]["name"] + ";\n")

        fp.write("\nvoid icc_init(void);\n")
        fp.write("\n#endif // ICC_H\n")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = ICCDataframeApp()
    MAIN_WINDOW = w
    w.show()
    sys.exit(app.exec_())
