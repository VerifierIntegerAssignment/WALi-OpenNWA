unsigned int tick = 0;

void start(int n, int y)
{
  while (n<0) {
    n++;
    y+=1000;
    while (y>=100) {
      tick++;
	  y-=100;
	}
  }
}

int main() 
{
	start(-20, 30);
	return 0;
}
