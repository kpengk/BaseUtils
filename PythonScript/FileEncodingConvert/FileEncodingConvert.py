import os
import codecs
import chardet


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
        print('Error: {0}'.format(e))
        return None


def convert(file, in_enc='GBK', out_enc='UTF-8'):
    in_enc = in_enc.upper()
    out_enc = out_enc.upper()
    if in_enc == out_enc:
        print('文件不需要转换.[' + file + ']')
        return
    try:
        print('转换 [' + file.split('\\')[-1] + ' ].....From ' + in_enc + ' --> ' + out_enc)
        fi = codecs.open(file, 'r', in_enc)
        new_content = fi.read()
        fi.close()
        fo = codecs.open(file, 'w', out_enc)
        fo.write(new_content)
        fo.close()
    except IOError as err:
        print('I/O error: {0}'.format(err))
    except Exception as e:
        print('Error: {0}'.format(e))


def convert_gbk_utf_8(path, covert_type):
    in_code = detect_code(path)
    if (in_code is None) or (in_code.upper() == 'WINDOWS-1254'):
        print('跳过 [' + path.split('\\')[-1] + ']')
        return
    if covert_type == '1':
        convert(path, in_code, 'UTF-8')
    else:
        convert(path, in_code, 'GBK')


if __name__ == '__main__':
    convert_type = input('目标文件格式, [1.UTF-8, 2.GBK]:')
    path = input('需要转换的文件或文件夹:')
    if os.path.isdir(path):
        filter_file = input('需要转换的文件后缀名，如[.h .cpp], 默认转换所有格式文件:')
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

    input('转换完成，按任意键退出...')


