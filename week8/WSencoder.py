import binascii

def text_to_bits(text, encoding='utf-8', errors='surrogatepass'):
    bits = bin(int(binascii.hexlify(text.encode(encoding, errors)), 16))[2:]
    return bits.zfill(8 * ((len(bits) + 7) // 8))

def int2bytes(i):
    hex_string = '%x' % i
    n = len(hex_string)
    return binascii.unhexlify(hex_string.zfill(n + (n & 1)))

def prep_original(ofile):
    lines = ofile.split('.')
    i = 0
    for i in range(len(lines)):
        lines[i] += '.\n' 
    return lines

def main():
    original = input("What is the original file name    : ")
    hidden =   input("What would be the hidden file name: ")
    message =  input("What is the secret message?       : ")
    ofile = open(original, 'r') #original file
    hfile = open(hidden, 'w')   #hidden message file
    lines = ofile.readlines()
    h_bits = text_to_bits(message)
    
    #+++++++++++++++++++++++++++++++++++++
    #   YOUR CODE GOES HERE
    # Here, you should include a white space at the end of each sentence
    # given in the variable: lines
    # the secret message is already provided in a binary form in the variable:
    # h_bits
    # 0 - no white space, 1 - add a white space
    # These are written to the hfile.
    #+++++++++++++++++++++++++++++++++++++
    
    ofile.close()
    hfile.close()
    return 0


if __name__ == "__main__":
    main()