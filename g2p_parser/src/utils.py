
def edit_distance(s1: str, s2: str) -> int:
	"""Calculate the Levenshtein distance between two strings."""
	m = len(s1)
	n = len(s2)
	if m > n:
		s1, s2 = s2, s1
		m, n = n, m
	return _edit_dist(s1, s2, m, n)

def _edit_dist(s1: str, s2: str, m: int, n: int) -> int:
	if m == 0:
		return n
	if n == 0:
		return m
	if s1[m-1] == s2[n-1]:
		return _edit_dist(s1, s2, m-1, n-1)
	return 1 + min(
		_edit_dist(s1, s2, m-1, n),    # Deletion
		_edit_dist(s1, s2, m, n-1),    # Insertion
		_edit_dist(s1, s2, m-1, n-1)   # Substitution
	)