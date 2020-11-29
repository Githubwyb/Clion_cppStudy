/*
 * @Author WangYubo
 * @Date 09/20/2018
 * @Description Some common algorithm
 */

#ifndef CLION_CPPSTUDY_ALGORITHM_HPP
#define CLION_CPPSTUDY_ALGORITHM_HPP

__inline int leftChild(int i) {
    return (2 * i + 1);
}

class Sort {

public:

    /*
     * @function 用插入排序数组
     * @param data 数组首地址
     * @param length 数组长度
     * @param order 排列顺序：顺序，true；倒序，false
     */
    template<typename T>
    static void insertionSort(T *data, int length, bool order = true) {
        T tmp = 0;
        for (int i = 1; i < length; ++i) {
            tmp = data[i];
            int j = 0;
            for (j = i; j > 0 && ((data[j - 1] > tmp) ^ !order); --j) {
                data[j] = data[j - 1];
            }
            data[j] = tmp;
        }
    }

    /*
     * @function 用希尔增量的希尔排序数组
     * @param data 数组首地址
     * @param length 数组长度
     * @param order 排列顺序：顺序，true；倒序，false
     */
    template<typename T>
    static void shellSort(T *data, int length, bool order = true) {
        T tmp = 0;
        for (int increment = length / 2; increment > 0; increment /= 2) {
            for (int i = increment; i < length; ++i) {
                tmp = data[i];
                int j = 0;
                for (j = i; j >= increment && ((data[j - increment] > tmp) ^ !order); j -= increment) {
                    data[j] = data[j - increment];
                }
                data[j] = tmp;
            }
        }
    }

    /*
     * @function 用堆排序数组
     * @param data 数组首地址
     * @param length 数组长度
     * @param order 排列顺序：顺序，true；倒序，false
     */
    template<typename T>
    static void heapSort(T *data, int length, bool order = true) {
        for (int i = length / 2; i >= 0 ; --i) {
            percDown(data, i, length);
        }

        for (int i = length - 1; i > 0; --i) {

        }
    }

private:
    /*
     * @function 堆排序的字函数
     * @param data 数组首地址
     * @param i 索引
     * @param length 数组长度
     * @param order 排列顺序：顺序，true；倒序，false
     */
    template<typename T>
    static void percDown(T *data, int i, int length, bool order = true) {

    }
};


#endif //CLION_CPPSTUDY_ALGORITHM_HPP
