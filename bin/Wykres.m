clc
clear

fileID1 = fopen('prawy1.txt','r');
fileID2 = fopen('lewy1.txt','r');
formatSpec = '%d %f';
sizeA = [2 Inf];
sizeB = [2 Inf];

A = fscanf(fileID1,formatSpec,sizeA);
B=fscanf(fileID2,formatSpec,sizeB);
fclose(fileID1);
fclose(fileID2);

A=A';
B=B';
a=size(A);
b=size(B);

for i=1:a
    if(A(i,2)==0)
      A(i,2)=2^20;  
    end
    if(B(i,2)==0)
      B(i,2)=2^20;  
    end
end

dB1 = mag2db(A(:,2))./2;
dB2 = mag2db(B(:,2))./2;

x1= [20 20000];
y1= [0 0];

plot(A(:,1), dB1,'r')
hold on
plot(B(:,1), dB2,'g')
line(x1,y1,'Color','black','LineStyle','--')
axis([20 20000 -5 55])
set(gca,'Ydir','reverse')
grid on
legend('Prawe ucho','Lewe ucho','Próg s³yszenia','Location','southwest')
xlabel('Czêstotliwoœæ[Hz]')
ylabel('Poziom s³yszenia[dB]')
title('Audiogram');