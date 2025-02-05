package com.galois.hacrypto.crypto;

import javax.crypto.Cipher;

import com.galois.hacrypto.req.output.Output;
import com.galois.hacrypto.test.Util;

public class Rng {

	private byte[] seed;
	private byte[] key;
	private byte[] iv;
	private final String algorithm;

	public Rng(byte[] seed, byte[] key, String algorithm) {
		this.seed = seed.clone();
		this.key = key.clone();
		this.algorithm = algorithm;
		/*
		 * if(this.seed.length != this.key.length){ throw new
		 * RuntimeException("Seed length and key length must match"); }
		 */
		iv = new byte[seed.length];
	}

	// TODO: The FIPS tests force you to expose an extra API
	// the user api would not take dt and instead somehow generate
	// it from the system date time string
	public byte[] nextRandom(byte[] dt) {

		byte[] i = Output.cipherBouncyCastle(
				algorithm + "/CBC/NoPadding", Cipher.ENCRYPT_MODE, key, iv, dt);

		byte[] rand = Output.cipherBouncyCastle(
				algorithm + "/CBC/NoPadding", Cipher.ENCRYPT_MODE, key, iv,
				Util.xor(i, seed));

		this.seed = Output.cipherBouncyCastle(
				algorithm + "/CBC/NoPadding", Cipher.ENCRYPT_MODE, key, iv,
				Util.xor(i, rand));

		return rand;
	}

	public static void main(String args[]) {
		// first test from AES128 test vector
		byte[] key = Util
				.hexStringToByteArray("36e287ef813afb5591ac09ad1d71e54d");
		byte[] dt = Util
				.hexStringToByteArray("562833b01d8db8f0b78f90afe193473e");
		byte[] seed = Util
				.hexStringToByteArray("80000000000000000000000000000000");
		Rng rng = new Rng(seed, key, "AES");
		String result = Util.byteArrayToHexString(rng.nextRandom(dt));
		if (result.toLowerCase().equals("7c4d77736f0b37068ae4861de69b88ff")) {
			System.out.println("AES128 passed");
		} else {
			System.out.println("AES128 failed, got " + result + ", expected " + 
		                       "7c4d77736f0b37068ae4861de69b88ff");	
		}

		// first test from TDES3 test vector
		key = Util
				.hexStringToByteArray("2f4c67e95db96e2538160e5ef419aecd671645ad89f1388c");
		dt = Util
				.hexStringToByteArray("3703f397fec2bd63");
		seed = Util
				.hexStringToByteArray("8000000000000000");
		rng = new Rng(seed, key, "DESede");
	 	result = Util.byteArrayToHexString(rng.nextRandom(dt));
		if (result.toLowerCase().equals("6e194f8d1a2a468b")) {
			System.out.println("DESede passed");
		} else {
			System.out.println("DESede failed, got " + result + ", expected " + 
		                       "6e194f8d1a2a468b");
		}
	}

}
