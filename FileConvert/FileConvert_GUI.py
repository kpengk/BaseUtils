from tkinter import *
from tkinter import filedialog
import os
import codecs
import chardet
import threading
import time


def find_files(path, list_files):
    for file in os.listdir(path):
        file_path = os.path.join(path, file)
        if os.path.isdir(file_path):
            find_files(file_path, list_files)
        else:
            list_files.append(file_path)
    return list_files


# 获取路径下所有的文件夹和文件
def list_files(path):
    all_files = []
    find_files(path, all_files)
    return all_files


def detect_code(file_path):
    try:
        with open(file_path, 'rb') as f:
            data = f.read()
            code_type = chardet.detect(data)['encoding']
            return code_type
    except Exception as e:
        append_tip('Error: {0}'.format(e))
        return None


def convert(file, in_enc='GBK', out_enc='UTF-8'):
    in_enc = in_enc.upper()
    out_enc = out_enc.upper()
    if in_enc == out_enc:
        append_tip('文件不需要转换.[' + file + ']')
        return
    try:
        append_tip('转换 [' + file.split('\\')[-1] + ' ].....From ' + in_enc + ' --> ' + out_enc)
        fi = codecs.open(file, 'r', in_enc)
        new_content = fi.read()
        fi.close()
        fo = codecs.open(file, 'w', out_enc)
        fo.write(new_content)
        fo.close()
    except IOError as err:
        append_tip('I/O error: {0}'.format(err))
    except Exception as e:
        append_tip('Error: {0}'.format(e))


def convert_gbk_utf_8(path, covert_type):
    in_code = detect_code(path)
    if (in_code is None) or (in_code.upper() == 'WINDOWS-1254'):
        append_tip('跳过 [' + path.split('\\')[-1] + ']')
        return
    if covert_type == '1':
        convert(path, in_code, 'UTF-8')
    else:
        convert(path, in_code, 'GBK')


def convert_all(convert_type, path, filter_file):
    if os.path.isdir(path):
        files = list_files(path)
        if len(filter_file) == 0:
            for file_name in files:
                convert_gbk_utf_8(file_name, convert_type)
        else:
            for file_name in files:
                suffix = os.path.splitext(file_name)[-1]
                if suffix in filter_file:
                    convert_gbk_utf_8(file_name, convert_type)
    else:
        convert_gbk_utf_8(path, convert_type)

    append_tip('转换完成')


e_path = None
e_filter = None
target_type = '1'
t_tip = None


def ui_process():
    root = Tk()
    root.title('文本编码转换')
    root.geometry('587x335+500+200')
    # 标签
    l_label_1 = Label(root, text='目标文件类型')
    l_label_1.place(x=10, y=10)
    # 单选按钮
    v = IntVar()
    r_utf8 = Radiobutton(root, text='UTF-8', variable=v, value=1, command=lambda: convert_type(1))
    r_utf8.place(x=120, y=10)
    r_utf8.select()
    r_gbk = Radiobutton(root, text='GBK', variable=v, value=2, command=lambda: convert_type(2)).place(x=220, y=10)
    # 标签
    l_label_2 = Label(root, text='需要转换的文件夹')
    l_label_2.place(x=10, y=40)
    global e_path
    e_path = Entry(root, show=None)
    e_path.place(x=115, y=40, width=400)
    # 选择文件夹按钮
    b_select_file = Button(root, text='选择', command=open_dir)
    b_select_file.place(x=520, y=40)
    # 标签
    l_label_3 = Label(root, text='需转换文件后缀名')
    l_label_3.place(x=10, y=80)
    global e_filter
    e_filter = Entry(root, show=None)
    e_filter.place(x=115, y=80, width=400)
    e_filter.insert(0, '.h .hpp .c .cpp')
    # 开始转换按钮
    b_start = Button(root, text='转换', command=start_convert)
    b_start.place(x=520, y=110)
    # 文本框
    global t_tip
    t_tip = Text(root, height=12)
    t_tip.place(x=10, y=150)

    mainloop()


def convert_type(index):
    global target_type
    target_type = str(index)


# 文件操作对话框
def open_dir():
    str_dir = filedialog.askdirectory(title='选择文件夹')
    global e_path
    e_path.delete(0, len(e_path.get()))
    e_path.insert(0, str_dir)


def append_tip(txt):
    global t_tip
    str_time = time.strftime('[%Y-%m-%d %H:%M:%S] ', time.localtime())
    t_tip.insert(INSERT, str_time + txt + '\n')


def start_convert():
    global e_path
    global e_filter
    global target_type
    path = e_path.get()
    filter_file = e_filter.get()
    if len(path) == 0:
        append_tip('请选择需要转换的文件夹')
        return
    convert_thread = threading.Thread(target=convert_all,
                                      kwargs={'convert_type': target_type, 'path': path, 'filter_file': filter_file})
    convert_thread.start()


if __name__ == "__main__":
    ui_process()
