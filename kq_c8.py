def calculate_average(scores):
    """
    Tính điểm trung bình của một danh sách điểm số.
    
    Hàm này nhận vào một danh sách các điểm số (có thể là số nguyên hoặc số thực),
    tính tổng tất cả các điểm, sau đó chia cho số lượng điểm để tính điểm trung bình.
    Nếu danh sách rỗng (không có điểm nào), hàm sẽ trả về 0 để tránh lỗi chia cho 0.
    
    Args:
        scores (list): Danh sách các điểm số cần tính trung bình.
                      Có thể chứa số nguyên (int) hoặc số thực (float).
                      Ví dụ: [8.5, 9.0, 7.5, 10.0] hoặc [85, 90, 75, 100]
    
    Returns:
        float: Điểm trung bình của tất cả các điểm trong danh sách.
               Nếu danh sách rỗng, trả về 0.
               Ví dụ: Với [8.5, 9.0, 7.5, 10.0] sẽ trả về 8.75
    
    Examples:
        >>> calculate_average([8.5, 9.0, 7.5, 10.0])
        8.75
        >>> calculate_average([85, 90, 75, 100])
        87.5
        >>> calculate_average([])
        0
    """
    # Khởi tạo biến tổng và biến đếm với giá trị ban đầu là 0
    # total: lưu tổng của tất cả các điểm
    # count: đếm số lượng điểm trong danh sách
    total = 0
    count = 0
    
    # Duyệt qua từng điểm số trong danh sách
    for score in scores:
        # Cộng điểm hiện tại vào tổng
        total += score
        # Tăng số lượng điểm lên 1 sau mỗi lần duyệt
        count += 1
    
    # Kiểm tra trường hợp đặc biệt: nếu danh sách rỗng (count = 0)
    # Tránh lỗi chia cho 0, trả về 0 ngay lập tức
    if count == 0:
        return 0
    
    # Tính và trả về điểm trung bình = tổng điểm chia cho số lượng điểm
    return total / count

